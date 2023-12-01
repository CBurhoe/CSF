#include <sstream>
#include <cctype>
#include <cassert>
#include <cstring>
#include <string>
#include "csapp.h"
#include "message.h"
#include "connection.h"

Connection::Connection()
  : m_fd(-1)
  , m_last_result(SUCCESS) {
}

Connection::Connection(int fd)
  : m_fd(fd)
  , m_last_result(SUCCESS) {
  // TODO: call rio_readinitb to initialize the rio_t object
  Rio_readinitb(&m_fdbuf, fd);
}

void Connection::connect(const std::string &hostname, int port) {
  // TODO: call open_clientfd to connect to the server
  const char* hn = hostname.c_str();
  const char* pn = std::itoa(port);
  this->m_fd = Open_clientfd(hn, pn);
  // TODO: call rio_readinitb to initialize the rio_t object
  Rio_readinitb(this->m_fdbuf, this->m_fd);
}

Connection::~Connection() {
  // TODO: close the socket if it is open
  Close(this->m_fd);
}

bool Connection::is_open() const {
  // TODO: return true if the connection is open
  return (this->m_fd > -1);
}

void Connection::close() {
  // TODO: close the connection if it is open
  Close(this->m_fd);
  this->m_fd = -1;
}

bool Connection::send(const Message &msg) {
  // TODO: send a message
  // return true if successful, false if not
  // make sure that m_last_result is set appropriately
  if (!is_open()) {
    this->m_last_result = EOF_OR_ERROR;
    return false;
  }

  size_t tag_length = msg.tag.length();
  size_t data_length = msg.data.length();
  if (tag_length + data_length + 1 > msg.MAX_LEN) {
    this->m_last_result = INVALID_MSG;
    return false;
  }
  char msg_to_server[msg.MAX_LEN + 1];
  
  msg.tag.copy(msg_to_server, tag_length);
  msg_to_server[tag_length] = ':';
  msg.data.copy(msg_to_server + tag_length + 1, data_length);
  msg_to_server[tag_length + data_length + 1] = '\0';
  size_t n = tag_length + data_length;
  
  if (Rio_writen(this->m_fd, msg_to_server, n) < 1) {
    this->m_last_result = EOF_OR_ERROR;
    return false;
  }
  
  this->m_last_result = SUCCESS;
  return true;
}

bool Connection::receive(Message &msg) {
  // TODO: receive a message, storing its tag and data in msg
  // return true if successful, false if not
  // make sure that m_last_result is set appropriately
  if (!is_open()) {
    this->m_last_result = EOF_OR_ERROR;
    return false;
  }
  char msg_from_server[msg.MAX_LEN + 1];
  //read line from server and store in msg_from_server
  //FIXME: need to check if '\n' is last char?
  if (Rio_readlineb(this->m_fdbuf, msg_from_server, msg.MAX_LEN) < 1) {
    this->m_last_result = EOF_OR_ERROR;
    return false;
  }
  //split input into tag and data in msg
  std::string message_string(msg_from_server);
  size_t split_on = message_string.find(':');
  msg.tag = message_string.substr(0, split_on);
  msg.data = message_string.substr(split_on + 1);
  
  this->m_last_result = SUCCESS;
  return true;
}
