#include <pthread.h>
#include <iostream>
#include <sstream>
#include <memory>
#include <set>
#include <vector>
#include <cctype>
#include <cassert>
#include <cstring>
#include <cstdlib>
#include <iostream>
#include <map>
#include <string>
#include "message.h"
#include "client_util.h"
#include "connection.h"
#include "user.h"
#include "room.h"
#include "guard.h"
#include "server.h"

////////////////////////////////////////////////////////////////////////
// Server implementation data types
////////////////////////////////////////////////////////////////////////

// add any additional data types that might be helpful
//       for implementing the Server member functions

struct ClientInfo {
    int client_fd;
    Connection *conn;
    struct User *usr;
    Room *rm;
    bool in_room = false;
    Server *server;
};

////////////////////////////////////////////////////////////////////////
// Client thread functions
////////////////////////////////////////////////////////////////////////

namespace {

void chat_with_sender(void *arg) {
  struct ClientInfo *info = static_cast<struct ClientInfo*>(arg);
  info->in_room = false;
  while(1) {
    struct Message new_message;
    struct Message server_response;
    if (!info->conn->receive(new_message)) {
      //handle failed receive
    }
    if (new_message.tag == TAG_JOIN) {
      // register to room
      if (info->in_room) {
        server_response.tag = TAG_ERR;
        server_response.data = "Must leave current room before joining another.";
      } else {
        Room *room_to_register = info->server->find_or_create_room(new_message.data);
        info->rm = room_to_register;
        room_to_register->add_member(info->usr);
        info->in_room = true;
        server_response.tag = TAG_OK;
        server_response.data = "Joined room.";
      }
    } else if (new_message.tag == TAG_LEAVE) {
      // de-register from room
      if (info->in_room) {
        info->rm->remove_member(info->usr);
        info->rm = NULL;
        info->in_room = false;
        server_response.tag = TAG_OK;
        server_response.data = "Left room.";
      } else {
        server_response.tag = TAG_ERR;
        server_response.data = "Not in a room.";
      }
    } else if (new_message.tag == TAG_QUIT) {
      // tear down thread (client quit)
      break;
    } else if (new_message.tag == TAG_SENDALL) {
      // sync and broadcast message to room
      if (!info->in_room) {
        server_response.tag = TAG_ERR;
        server_response.data = "Must join room before sending messages.";
      } else {
        info->rm->broadcast_message(info->usr->username, new_message.data);
        server_response.tag = TAG_OK;
        server_response.data = "Message delivered.";
      }
    }
    if (!info->conn->send(server_response)) {
      // Handle failed send
    }
  }
}

void chat_with_receiver(void *arg) {
  struct ClientInfo *info = static_cast<struct ClientInfo*>(arg);
  struct Message new_message;
  if (!info->conn->receive(new_message)) {
    // Handle failed receive
  }
  if (new_message.tag != TAG_JOIN) {
    struct Message server_response = Message(TAG_ERR, "Must join room.");
    if (!info->conn->send(server_response)) {
      // Handle failed send
    }
    return;
  }
  Room *room_to_register = info->server->find_or_create_room(new_message.data);
  info->rm = room_to_register;
  room_to_register->add_member(info->usr);
  info->in_room = true;
  struct Message server_response;
  server_response.tag = TAG_OK;
  server_response.data = "Joined room.";
  if (!info->conn->send(server_response)) {
    // Handle failed send
  }
  
  //continuously look for messages
  while(1) {
    struct Message *new_delivery = info->usr->mqueue.dequeue();
    if (new_delivery == nullptr) {
      continue;
    }
    if (!info->conn->send(*new_delivery)) {
      // Handle failed send
    }
  }
}

void *worker(void *arg) {
  pthread_detach(pthread_self());

  //  use a static cast to convert arg from a void* to
  //       whatever pointer type describes the object(s) needed
  //       to communicate with a client (sender or receiver)
  struct ClientInfo *info = static_cast<struct ClientInfo*>(arg);

  // read login message (should be tagged either with
  //       TAG_SLOGIN or TAG_RLOGIN), send response
  struct Message login_msg;
  if (!info->conn->receive(login_msg)) {
    // handle failed read
  }
  User new_user(login_msg.data);
  info->usr = &new_user;
  // depending on whether the client logged in as a sender or
  //       receiver, communicate with the client (implementing
  //       separate helper functions for each of these possibilities
  //       is a good idea)
  struct Message server_response;
  
  if (login_msg.tag == TAG_SLOGIN) {
    server_response.tag = TAG_OK;
    server_response.data = "Logged in.";
    if (!info->conn->send(server_response)) {
      // Handle failed send
    }
    chat_with_sender(info);
  } else if (login_msg.tag == TAG_RLOGIN) {
    server_response.tag = TAG_OK;
    server_response.data = "Logged in.";
    if (!info->conn->send(server_response)) {
      // Handle failed send
    }
    info->usr->is_receiver = true;
    chat_with_receiver(info);
  } else {
    server_response.tag = TAG_ERR;
    server_response.data = "Not logged in";
    if (!info->conn->send(server_response)) {
      // Handle failed send
    }
  }
  server_response.tag = TAG_OK;
  server_response.data = "Client quit.";
  if (!info->conn->send(server_response)) {
    // Handle failed send
  }
  delete info->conn;
  free(info);
  return nullptr;
}

}

////////////////////////////////////////////////////////////////////////
// Server member function implementation
////////////////////////////////////////////////////////////////////////

Server::Server(int port)
  : m_port(port)
  , m_ssock(-1) {
  // initialize mutex
  pthread_mutex_init(&this->m_lock, NULL);
}

Server::~Server() {
  // destroy mutex
  pthread_mutex_destroy(&this->m_lock);
}

bool Server::listen() {
  // use open_listenfd to create the server socket, return true
  //       if successful, false if not
  std::string s = std::to_string(m_port);
  const char* pn = s.c_str();
  this->m_ssock = Open_listenfd(pn);
  return (this->m_ssock > 0);
}

void Server::handle_client_requests() {
  // infinite loop calling accept or Accept, starting a new
  //       pthread for each connected client
  while(1) {
    int c_fd = Accept(this->m_ssock, NULL, NULL);
    if (c_fd < 0) {
      std::cerr << "Error accepting client connection.\n";
    }
    
    struct ClientInfo *info = static_cast<struct ClientInfo*>(malloc(sizeof(struct ClientInfo)));
    info->conn = new Connection(c_fd);
    info->server = this;
    
    pthread_t thread_id;
    
    Pthread_create(&thread_id, NULL, worker, info);
  }
}

Room *Server::find_or_create_room(const std::string &room_name) {
  // return a pointer to the unique Room object representing
  //       the named chat room, creating a new one if necessary
  {
    Guard(this->m_lock);
    auto search_rooms = this->m_rooms.find(room_name);
    if (search_rooms != this->m_rooms.end()) {
      return search_rooms->second;
    } else {
      Room *new_room = new Room(room_name);
      this->m_rooms.insert(std::make_pair(room_name, new_room));
      return new_room;
    }
  }
}
