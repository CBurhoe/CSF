#include "guard.h"
#include "message.h"
#include "message_queue.h"
#include "user.h"
#include "room.h"

Room::Room(const std::string &room_name)
  : room_name(room_name) {
  // TODO: initialize the mutex
  pthread_mutex_init(&lock, NULL);
}

Room::~Room() {
  // TODO: destroy the mutex
  pthread_mutex_destroy(&lock);
}

void Room::add_member(User *user) {
  // TODO: add User to the room
  {
    Guard(this->lock);
    this->members.insert(user);
  }
}

void Room::remove_member(User *user) {
  // TODO: remove User from the room
  {
    Guard(this->lock);
    auto it = members.find(user);
    if (it != members.end()) {
      members.erase(it);
    }
  }
}

void Room::broadcast_message(const std::string &sender_username, const std::string &message_text) {
  // TODO: send a message to every (receiver) User in the room
  struct Message *new_message = new Message();
  new_message->tag = TAG_DELIVERY;
  new_message->data = this->room_name + ":" + sender_username + ":" + message_text;
  { //lock the room using a guard during the critical section
    Guard(this->lock);
    for (User *usr: this->members) {
      if (usr->is_receiver) {
        usr->mqueue.enqueue(new_message);
      }
    }
  }
}
