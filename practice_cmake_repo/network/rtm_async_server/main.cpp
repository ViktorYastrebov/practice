#include <iostream>
#include "server.hpp"

#include "message.h"
#include "os_info.h"
#include "request.hpp"

#include <random>

//let's try to make the follwoing concept:
// bind handle_write/read for request/responce itself. + gives the oppotunity for mark it as done.
// may be it will need to create the separed thread - std::future<>.
// there might be a question for compound task such as vector<task> etc.

//TODO: moved to the std::packaged_task;

//what if we determine action by applying request/responce
int main(int argc, char *argv[]) {
  std::string ip("127.0.0.1");
  unsigned short port = 2737;
  server serv(ip, port);
  serv.start();

  std::string  command;

  const std::string message_prefix("message:");
  const std::string os_info_prefix("os_info:");

  while (std::getline(std::cin, command)) {
    if (command == "quit") {
      return 0;
    }

    if(command.substr(0, message_prefix.length()) == message_prefix) {
      std::string msg_s(command.substr(message_prefix.length()));
      std::shared_ptr<message> msg = std::make_shared<message>(msg_s);
      serv.assign_to_all(msg);
    }

    if(command.substr(0, os_info_prefix.length()) == os_info_prefix) {
      std::shared_ptr<os_info> osi = std::make_shared<os_info>();
      serv.assign_to_all(osi);
    }
  }
  return 0;
}
