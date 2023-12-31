#include <iostream>
#include <string>
#include <sstream>
#include <stdexcept>
#include "csapp.h"
#include "message.h"
#include "connection.h"
#include "client_util.h"

int main(int argc, char **argv) {
  if (argc != 4) {
    std::cerr << "Usage: ./sender [server_address] [port] [username]\n";
    return 1;
  }

  std::string server_hostname;
  int server_port;
  std::string username;

  server_hostname = argv[1];
  server_port = std::stoi(argv[2]);
  username = argv[3];

  // connect to server
  Connection conn;
  struct Message server_response;
  conn.connect(server_hostname, server_port);
  // send slogin message
  struct Message s_login = Message(TAG_SLOGIN, username);
  if (!conn.send(s_login)) {
    std::cerr << "Message to server failed to send.\n";
    exit(2);
  }
  if (!conn.receive(server_response)) {
    std::cerr << "Failed to receive a response from server.\n";
    exit(2);
  }
  if (server_response.tag == TAG_ERR) {
    // handle failed s_login
    std::cerr << server_response.data;
    exit(3);
  }
  
  // loop reading commands from user, sending messages to
  //       server as appropriate
  std::string input;
  // read in line of input from user
  while(std::getline(std::cin, input)) {
    struct Message new_message;
    // check if message or command and handle accordingly
    if (input[0] != '/') {
      new_message.tag = TAG_SENDALL;
      new_message.data = input.substr(0, Message::MAX_LEN);
    } else if (input.compare(0, 6, "/join ") == 0) {
      new_message.tag = TAG_JOIN;
      new_message.data = input.substr(6);
    } else if (input.compare(0, 6, "/leave") == 0) {
      new_message.tag = TAG_LEAVE;
      new_message.data = "";
    } else if (input.compare(0, 5, "/quit") == 0) {
      new_message.tag = TAG_QUIT;
      new_message.data = "";
    } else {
      std::cerr << "Invalid command.\n";
      continue;
    }
    
    if (!conn.send(new_message)) {
      // handle failed send
      std::cerr << "Message to server failed to send.\n";
      exit(2);
    }
    if (!conn.receive(server_response)) {
      // handle failed receive
      std::cerr << "Failed to receive a response from server.\n";
      exit(2);
    }
    if (server_response.tag == TAG_ERR) {
      std::cerr << server_response.data;
      continue;
    } else if ((server_response.tag == TAG_OK) && (new_message.tag == TAG_QUIT)) {
      exit(0);
    } else if (server_response.tag == TAG_OK) {
      continue;
    }
  }
  return 0;
}
