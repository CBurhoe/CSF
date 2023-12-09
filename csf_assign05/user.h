#ifndef USER_H
#define USER_H

#include <string>
#include "message_queue.h"

struct User {
  std::string username;
  bool is_receiver = false;
  // queue of pending messages awaiting delivery
  MessageQueue mqueue;

  User(const std::string &username) : username(username) { }
};

#endif // USER_H
