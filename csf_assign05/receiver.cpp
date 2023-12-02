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
  std::string server_hostname = argv[1];
  int server_port = std::stoi(argv[2]);
  std::string username = argv[3];
  std::string room_name = argv[4];

  Connection conn;
  struct Message server_response;
  
  // connect to server
  //connect via hostname and port number
  conn.connect(server_hostname, server_port);
  // send rlogin and join messages (expect a response from
  //       the server for each one)
  struct Message r_login = Message(TAG_RLOGIN, username);
  if (!conn.send(r_login)) {
    // handle failed send
    std::cerr << "Message to server failed to send.\n";
    exit(2);
  }
  // listen for response
  if (!conn.receive(server_response)) {
    // handle failed receive
    std::cerr << "Failed to receive a response from server.\n";
    exit(2);
  }
  if (server_response.tag == TAG_ERR) {
    // handle failed r_login
    std::cerr << server_response.data;
    exit(3);
  }
  struct Message join = Message(TAG_JOIN, room_name);
  if (!conn.send(join)) {
    // handle failed send
    std::cerr << "Message to server failed to send.\n";
    exit(2);
  }
  // listen for response
  if (!conn.receive(server_response)) {
    // handle failed receive
    std::cerr << "Failed to receive a response from server.\n";
    exit(2);
  }
  if (server_response.tag == TAG_ERR) {
    // handle failed join
    std::cerr << server_response.data;
    exit(3);
  }
  // loop waiting for messages from server
  //       (which should be tagged with TAG_DELIVERY)
  struct Message new_message;
  while(true) {
    if (!conn.receive(new_message)) {
      // handle failed receive
      std::cerr << "Failed to receive a response from server.\n";
      exit(2);
    }
    if (new_message.tag == TAG_ERR) {
      // handle error case
      std::cerr << new_message.data;
      exit(3);
    } else if (new_message.tag == TAG_DELIVERY) {
      // handle delivered message from user
      //usage: delivery:room:sender:message_text
      std::vector<std::string> delivery_payload = new_message.split_delivery_payload();
      std::string sender = delivery_payload[1];
      std::string message_text = delivery_payload[2];
      std::cout << sender << ": " << message_text;
    }
  }

  return 0;
}
