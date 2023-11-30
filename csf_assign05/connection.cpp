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
  Rio_readinitb(this->m_fdbuf, fd);
}

void Connection::connect(const std::string &hostname, int port) {
  // TODO: call open_clientfd to connect to the server
  const char* hn = hostname.c_str();
  const char* pn = itoa(port);
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
  }
  std::string message_to_server;
  
}

bool Connection::receive(Message &msg) {
  // TODO: receive a message, storing its tag and data in msg
  // return true if successful, false if not
  // make sure that m_last_result is set appropriately
  if (!is_open()) {
    this->m_last_result = EOF_OR_ERROR;
  }
  
}
