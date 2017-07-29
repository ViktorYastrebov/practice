#include <stdio.h>
#include <sys/socket.h>
#include <sys/types.h>

#include <iostream>
#include <cstring>

#include "socket.h"

auto main(int argc, char * argv[]) -> int {
  if(argc < 3) {
    std::cout << "usage: " << argv[0]  << " <host> <port>" << std::endl;
    return -1;
  }
  std::string addr(argv[1]);
  int port = std::stoi(argv[2]);

  Socket client(addr, port);
  client.connect();

  std::cout << "enter message or command :" << std::endl;
  std::string quit("quit");
  std::string cmd;

  try {
    while(std::getline(std::cin, cmd)) {
     if(cmd == quit) {
        std::cout << "closing connection ..." << std::endl;
        client << -1;
        break;
      }
      std::string response;
      client << cmd;
      client >> response;
      std::cout << "responce :" << response;
      std::cout << ", msgs are equal :" << std::boolalpha  << (cmd == response) << std::endl;
    }
  } catch (const std::exception  &ex) {
    std::cout << "error occurs : " << ex.what() << std::endl;
  }
  return 0;
}
