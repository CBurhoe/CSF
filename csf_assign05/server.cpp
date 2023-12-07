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
    std::string client_type;
    Connection *conn;
    struct User *usr;
    Room *rm;
    
    
    ClientInfo() { }
};
////////////////////////////////////////////////////////////////////////
// Client thread functions
////////////////////////////////////////////////////////////////////////

namespace {

void *worker(void *arg) {
  pthread_detach(pthread_self());

  // TODO: use a static cast to convert arg from a void* to
  //       whatever pointer type describes the object(s) needed
  //       to communicate with a client (sender or receiver)
  struct ClientInfo *info = arg;

  // TODO: read login message (should be tagged either with
  //       TAG_SLOGIN or TAG_RLOGIN), send response
  //buffer to store message line from fd
  char login_message[Message::MAX_LEN + 1];
  rio_t *reader;
  //initialize rio_t object
  Rio_readinitb(reader, info->client_fd);
  size_t max = Message::MAX_LEN;
  //read message from socket FIXME: handle read line error
  ssize_t message_len = Rio_readlineb(reader, login_message, max);
  if (message_len > max) {
    char response = "err:Invalid message: too long.\n";
    Rio_writen(info->client_fd, &response, 31);
  }
  //split input message to determine tag
  std::string message_string(login_message);
  size_t split_on message_string.find(':');
  std::string msg_tag = message_string.substr(0, split_on);
  std::string username = message_string.substr(split_on + 1);
  char response = "ok:Logged in\n";
  // TODO: depending on whether the client logged in as a sender or
  //       receiver, communicate with the client (implementing
  //       separate helper functions for each of these possibilities
  //       is a good idea)
  if (msg_tag.compare(TAG_SLOGIN) == 0) {
    chat_with_sender(info);
  } else if (msg_tag.compare(TAG_RLOGIN) == 0) {
    chat_with_receiver(info);
  }
  return nullptr;
}

void chat_with_sender(void *arg) {
  //TODO
}

void chat_with_receiver(void *arg) {
  //TODO
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
    
    //FIXME: find end of scope of info and free()
    struct ClientInfo *info = malloc(sifeof(struct ClientInfo));
    info->client_fd = c_fd;
    
    pthread_t thread_id;
    
    //FIXME: need to find end of thread scope and destroy zombie?
    Pthread_create(&thread_id, NULL, worker, info);
  }
}

Room *Server::find_or_create_room(const std::string &room_name) {
  // DONE: return a pointer to the unique Room object representing
  //       the named chat room, creating a new one if necessary
  auto search_rooms = this->m_rooms.find(room_name);
  if (search_rooms != this->m_rooms.end()) {
    return search_rooms.second();
  } else {
    Room *new_room = Room(room_name);
    this->m_rooms.insert({room_name, new_room});
    return new_room;
  }
}
