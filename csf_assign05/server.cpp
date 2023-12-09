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

// TODO: add any additional data types that might be helpful
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
  while(1) {
    struct Message new_message;
    struct Message server_response;
    if (!info->conn->receive(new_message)) {
      //TODO
    }
    if (new_message.tag == TAG_JOIN) {
      //TODO: register to room
      if (info->in_room) {
        server_response.tag = TAG_ERR;
        server_response.data = "Must leave current room before joining another.\n";
      } else {
        Room *room_to_register = info->server->find_or_create_room(new_message.data);
        info->rm = room_to_register;
        room_to_register->add_member(info->usr);
        info->in_room = true;
        server_response.tag = TAG_OK;
        server_response.data = "Joined room.\n";
      }
    } else if (new_message.tag == TAG_LEAVE) {
      //TODO: de-register from room
      if (info->in_room) {
        info->rm->remove_member(info->usr);
        info->rm = NULL;
        info->in_room = false;
        server_response.tag = TAG_OK;
        server_response.data = "Left room.\n";
      } else {
        server_response.tag = TAG_ERR;
        server_response.data = "Not in a room.\n";
      }
    } else if (new_message.tag == TAG_QUIT) {
      //TODO: tear down thread (client quit)
      break;
    } else if (new_message.tag == TAG_SENDALL) {
      //TODO: sync and broadcast message to room
      if (!info->in_room) {
        server_response.tag = TAG_ERR;
        server_response.data = "Must join room before sending messages.\n";
      } else {
        std::string msg = TAG_DELIVERY + ":" + new_message.data;
        info->rm->broadcast_message(info->usr->username, msg);
        server_response.tag = TAG_OK;
        server_response.data = "Message delivered.\n";
      }
    }
    if (!info->conn->send(server_response)) {
      //TODO: Handle failed send
    }
  }
  
  //TODO
}

void chat_with_receiver(void *arg) {
  struct ClientInfo *info = static_cast<struct ClientInfo*>(arg);
  struct Message new_message;
  if (!info->conn->receive(new_message)) {
    //TODO: Handle failed receive
  }
  //FIXME: shouldn't need to check this
  if (new_message.tag != TAG_JOIN) {
    struct Message server_response = Message(TAG_ERR, "Must join room.\n");
    if (!info->conn->send(server_response)) {
      //TODO: Handle failed send
    }
    return;
  }
  Room *room_to_register = info->server->find_or_create_room(new_message.data);
  info->rm = room_to_register;
  room_to_register->add_member(info->usr);
  info->in_room = true;
  struct Message server_response;
  server_response.tag = TAG_OK;
  server_response.data = "Joined room.\n";
  if (!info->conn->send(server_response)) {
    //TODO: Handle failed send
  }
  
  //continuously look for messages
  while(1) {
    struct Message *new_delivery = info->usr->mqueue.dequeue();
    if (new_delivery == nullptr) {
      continue;
    }
    if (!info->conn->send(*new_delivery)) {
      //TODO: Handle failed send
    }
  }
  //TODO
}

void *worker(void *arg) {
  pthread_detach(pthread_self());

  // TODO: use a static cast to convert arg from a void* to
  //       whatever pointer type describes the object(s) needed
  //       to communicate with a client (sender or receiver)
  struct ClientInfo *info = static_cast<struct ClientInfo*>(arg);

  // TODO: read login message (should be tagged either with
  //       TAG_SLOGIN or TAG_RLOGIN), send response
  struct Message login_msg;
  if (!info->conn->receive(login_msg)) {
    //TODO: handle failed read
  }
  User new_user(rtrim(login_msg.data));
  info->usr = &new_user;
  // TODO: depending on whether the client logged in as a sender or
  //       receiver, communicate with the client (implementing
  //       separate helper functions for each of these possibilities
  //       is a good idea)
  struct Message server_response;
  
  if (login_msg.tag == TAG_SLOGIN) {
    server_response.tag = TAG_OK;
    server_response.data = "Logged in\n";
    if (!info->conn->send(server_response)) {
      //TODO: Handle failed send
    }
    chat_with_sender(info);
  } else if (login_msg.tag == TAG_RLOGIN) {
    server_response.tag = TAG_OK;
    server_response.data = "Logged in\n";
    if (!info->conn->send(server_response)) {
      //TODO: Handle failed send
    }
    info->usr->is_receiver = true;
    chat_with_receiver(info);
  } else {
    server_response.tag = TAG_ERR;
    server_response.data = "Not logged in\n";
    if (!info->conn->send(server_response)) {
      //TODO: Handle failed send
    }
  }
  server_response.tag = TAG_OK;
  server_response.data = "Client quit.\n";
  if (!info->conn->send(server_response)) {
    //TODO: Handle failed send
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
  // TODO: initialize mutex
  pthread_mutex_init(this->m_lock, NULL);
}

Server::~Server() {
  // TODO: destroy mutex
  pthread_mutex_destroy(this->m_lock);
}

bool Server::listen() {
  // DONE: use open_listenfd to create the server socket, return true
  //       if successful, false if not
  std::string s = std::to_string(port);
  const char* pn = s.c_str();
  this->m_ssock = Open_listenfd(pn);
  return (this->m_ssock > 0);
}

void Server::handle_client_requests() {
  // DONE: infinite loop calling accept or Accept, starting a new
  //       pthread for each connected client
  while(1) {
    int c_fd = Accept(this->m_ssock, NULL, NULL);
    if (c_fd < 0) {
      std::cerr << "Error accepting client connection.\n";
    }
    
    struct ClientInfo *info = static_cast<struct ClientInfo*>(malloc(sizeof(struct ClientInfo)));
    info->conn = new Connection(c_fd);
    info->server = &this;
    
    pthread_t thread_id;
    
    Pthread_create(&thread_id, NULL, worker, info);
  }
}

Room *Server::find_or_create_room(const std::string &room_name) {
  // DONE: return a pointer to the unique Room object representing
  //       the named chat room, creating a new one if necessary
  {
    Guard(this->m_lock);
    auto search_rooms = this->m_rooms.find(room_name);
    if (search_rooms != this->m_rooms.end()) {
      return search_rooms.second();
    } else {
      Room *new_room = Room(room_name);
      this->m_rooms.insert({room_name, new_room});
      return new_room;
    }
  }
}
