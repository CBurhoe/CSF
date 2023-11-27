#include <iostream>
#include <string>
#include <vector>
#include <stdexcept>
#include "csapp.h"
#include "message.h"
#include "connection.h"
#include "client_util.h"

int main(int argc, char **argv) {
  if (argc != 5) {
    std::cerr << "Usage: ./receiver [server_address] [port] [username] [room]\n";
    return 1;
  }
  //FIXME: May need to check input string lengths
  std::string server_hostname = argv[1];
  int server_port = std::stoi(argv[2]);
  std::string username = argv[3];
  std::string room_name = argv[4];

  Connection conn;

  // TODO: connect to server
  //initialize server
  Server server = new Server(server_port);
  //connect via hostname and port number
  conn.connect(server_hostname, server_port);
  // TODO: send rlogin and join messages (expect a response from
  //       the server for each one)
  Message r_login = Message(TAG_RLOGIN, username); //fixme: initializing structs?
  conn.send(r_login);
  //TODO: listen for response
  Message join = Message(TAG_JOIN, room_name);
  conn.send(join);
  //TODO: listen for response
  
  // TODO: loop waiting for messages from server
  //       (which should be tagged with TAG_DELIVERY)


  return 0;
}
