#include <iostream>
#include "client.hpp"

#include "message.h"
#include "os_info.h"

//http://stackoverflow.com/questions/12794107/why-do-i-need-strand-per-connection-when-using-boostasio/12801042#12801042
//http://stackoverflow.com/questions/16397540/when-do-i-have-to-use-boostasiostrand
//http://stackoverflow.com/questions/1639286/boost-asio-async-read-doesnt-stop-reading

//https://boostgsoc13.github.io/boost.afio/doc/html/afio/reference/functions/read/async_read_3_length_deducing.html !!!

//client
int main(int argc, char *argv[]) {

  std::string host("127.0.0.1");
  std::string port("2737");

  client cl(host, port);
  
  cl.connect();

  std::string  command;
  while (std::getline(std::cin, command)) {

    if (command == "quit") {
      return 0;
    }
    std::shared_ptr<base_task> command_task = std::make_shared<message>(command);
    cl.send(command_task);
  }
  return 0;
}