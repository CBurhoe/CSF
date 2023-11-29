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
  Message r_login = Message(TAG_RLOGIN, username);
  if (!conn.send(r_login)) {
    //TODO: handle failed send
  }
  //TODO: listen for response
  if (!conn.receive(server_response)) {
    //TODO: handle failed receive
  }
  if (server_response.tag == TAG_ERR) {
    //DONE: handle failed r_login
    std::cerr << server_response.data << "\n";
    exit(3);
  }
  Message join = Message(TAG_JOIN, room_name);
  if (!conn.send(join)) {
    //TODO: handle failed send
  }
  //TODO: listen for response
  if (!conn.receive(server_response)) {
    //TODO: handle failed receive
  }
  if (server_response.tag == TAG_ERR) {
    //DONE: handle failed join
    std::cerr << server_response.data << "\n";
    exit(3);
  }
  // TODO: loop waiting for messages from server
  //       (which should be tagged with TAG_DELIVERY)
  while(true) {
    Message new_message;
    if (!conn.receive(new_message)) {
      //TODO: handle failed receive
    }
    if (new_message.tag == TAG_ERR) {
      //DONE: handle error case
      std::cerr << new_message.data << "\n";
      exit(3);
    } else if (new_message.tag == TAG_DELIVERY) {
      //TODO: handle delivered message from user
      //usage: delivery:room:sender:message_text
      std::vector delivery_payload = new_message.split_delivery_payload();
      std::string sender = delivery_payload[1];
      std::string message_text = delivery_payload[2];
      std::cout << sender << ": " << message_text << endl; //extra newline?
    }
  }

  return 0;
}
