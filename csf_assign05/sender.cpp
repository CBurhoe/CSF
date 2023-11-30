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

  // DONE: connect to server
  Connection conn;
  Message server_response;
  conn.connect(server_hostname, server_response);

  // DONE: send slogin message
  Message s_login= Message(TAG_SLOGIN, username);
  if (!conn.send(s_login)) {
    std::cerr << "Message to server"
  }
  if (!conn.receive(server_response)) {
    //TODO: handle failed receive
  }
  if (server_response.tag == TAG_ERR) {
    std::cerr << server_response.data << "\n";
    exit(3);
  }
  
  // TODO: loop reading commands from user, sending messages to
  //       server as appropriate
  std::string input;
  //read in line of input from user
  while(std::getline(std::cin, input)) {
    Message new_message;
    //check if message or command and handle accordingly
    if (input.compare(0, 0, '/') != 0) {
      new_message.tag = TAG_SENDALL;
      new_message.data = input.substr(0, new_message.MAX_LEN);
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
      //TODO: handle failed send
    }
    Message server_response;
    if (!conn.receive(server_response)) {
      //TODO: handle failed receive
    }
    if ((server_response.tag == TAG_ERR) && (new_message.tag == TAG_SLOGIN)) {
      std::cerr << server_response.data << "\n";
      exit(4);
    } else if (server_response.tag == TAG_ERR) {
      std::cerr << server_response.data << "\n";
      continue;
    } else if ((server_response.tag == TAG_OK) && (new_message.tag == TAG_QUIT)) {
      exit(0);
    } else if (server_response.tag == TAG_OK) {
      continue;
    }
  }
  return 0;
}
