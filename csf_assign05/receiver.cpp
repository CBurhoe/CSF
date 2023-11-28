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
  Message server_response;
  
  // DONE: connect to server
  //connect via hostname and port number
  conn.connect(server_hostname, server_port);
  // TODO: send rlogin and join messages (expect a response from
  //       the server for each one)
  Message r_login = Message(TAG_RLOGIN, username); //fixme: initializing structs?
  if (!conn.send(r_login)) {
    //TODO: handle failed send
  }
  //TODO: listen for response
  if (!conn.receive(server_response)) {
    //TODO: handle failed receive
  }
  if (!server_response.tag == TAG_OK) {
    //TODO: handle failed r_login
  }
  Message join = Message(TAG_JOIN, room_name);
  if (!conn.send(join)) {
    //TODO: handle failed send
  }
  //TODO: listen for response
  if (!conn.receive(server_response)) {
    //TODO: handle failed receive
  }
  if (!server_response.tag == TAG_OK) {
    //TODO: handle failed join
  }
  // TODO: loop waiting for messages from server
  //       (which should be tagged with TAG_DELIVERY)
  while(true) {
    Message new_message;
    if (!conn.receive(new_message)) {
      //TODO: handle failed receive
    }
    
  }

  return 0;
}
