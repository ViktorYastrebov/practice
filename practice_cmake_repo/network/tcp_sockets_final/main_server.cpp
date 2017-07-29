
#include <stdio.h>
#include <unistd.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <signal.h>

#include <stdexcept>
#include <fstream>

#include <iostream>

#include "common.h"
#include "socket.h"

void echo(std::shared_ptr<Socket>  conn) {
  while(true) {
    std::string str;
    (*conn) >> str;
    (*conn) << str; 
  }
}

void run(const std::string  &addr, int port) {
  Socket server_sock(addr, port);
  server_sock.listen();

  while(true) {
    std::cout << "waiting for connections ..." << std::endl;
    std::shared_ptr<Socket> conn = server_sock.accept();
    ::signal(SIGCHLD, SIG_IGN);

    pid_t  pid = fork();
    switch(pid) {
      case -1:
      {
        std::cout << "fork failed : " << getLastError() << std::endl;
      }break;
      case 0:
      {
        //child
        server_sock.close();
        try {
          echo(conn);
        } catch(const std::logic_error  &close_con) {
          conn->close();
        } catch(const std::exception  &ex) {
           std::string fn = "log_" + std::to_string(::getpid()) + ".log";
           std::ofstream ofs(fn.c_str()); 
           if(ofs) {
             ofs << "error occurs :" << ex.what() << std::endl;
           }
           conn->close();
           exit(1);
        }
        _exit(0);
      }break;
      default: {
        conn->close();
      }
    };
  }
}

auto main(int argc, char * argv[]) -> int {
  if(argc < 2) {
    std::cout << "usage: " <<  argv[0] << " <port number>" << std::endl;
    return 1;
  }

  std::string strPort(argv[1]);
  int port = std::stoi(strPort);
  std::string addr("127.0.0.1");

  run(addr, port);

  return 0;
}
