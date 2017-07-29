#include <unistd.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <arpa/inet.h>

#include <stdexcept>
#include <cstring>

#include "socket.h"
#include "common.h"

Socket::Socket(int sock)
  : sfd_(sock)
{}

Socket::Socket(const std::string  &addr, unsigned short port)
  :addr_(addr), port_(htons(port))
{
  sfd_ = socket(AF_INET, SOCK_STREAM, 0);
  if(sfd_ < 0 ) {
    throw std::runtime_error(getLastError());
  }
}

Socket::~Socket() {
  close();
}

void Socket::connect() const {
  struct sockaddr_in server_addr;
  std::memset(&server_addr, 0, sizeof(struct sockaddr_in));
  server_addr.sin_family = AF_INET;
  server_addr.sin_addr.s_addr = inet_addr(addr_.c_str());
  server_addr.sin_port = port_;

  int ret = ::connect(sfd_, (struct sockaddr *)&server_addr, sizeof(server_addr));
  if (ret < 0) {
    throw std::runtime_error(getLastError());
  }
}
 
void Socket::connect(const std::string  &addr, unsigned short port) {
  addr_ = addr;
  port_ = htons(port);
  connect();
}

void Socket::listen() const {
  struct sockaddr_in serv_addr;
  std::memset(&serv_addr,0, sizeof(struct sockaddr_in));
  serv_addr.sin_family = AF_INET;
  serv_addr.sin_port = port_;
  serv_addr.sin_addr.s_addr = inet_addr(addr_.c_str());

  if (::bind(sfd_, (struct sockaddr *) &serv_addr, sizeof(serv_addr)) < 0) {
    throw std::runtime_error(getLastError());
  }

  constexpr int max_pending_conns = 5;
  int ret = ::listen(sfd_, max_pending_conns);
  if(ret < 0) {
    throw std::runtime_error(getLastError());
  }
}

std::shared_ptr<Socket> Socket::accept() const {
  struct sockaddr_in client_addr;
  socklen_t client_len = sizeof(client_addr);
  int conn_socket_fd = ::accept(sfd_, (struct sockaddr *)&client_addr, &client_len);
  if(conn_socket_fd < 0) {
    throw std::runtime_error(getLastError()); 
  }
  std::shared_ptr<Socket> conn(new Socket(conn_socket_fd));
  return conn;
}

void Socket::close() {
  if(is_opened()) {
    ::close(sfd_);
  }
} 

bool Socket::is_opened() const {
  int err = 0;
  socklen_t size = sizeof(err);
  int check = ::getsockopt(sfd_, SOL_SOCKET, SO_ERROR, &err, &size);
  return check == 0;
}

Socket  & Socket::operator << (int value) {
  std::uint32_t val = value;
  std::uint32_t convVal = htonl(val);
  int ret = ::write(sfd_, static_cast<const void*>(&convVal), sizeof(convVal));
  if(ret < 0) {
    throw std::runtime_error(getLastError());
  }
  return *this;
}

Socket  & Socket::operator << (const std::string  &value) {
  int length = value.length();
  *this << length;
  int ret = ::write(sfd_, value.c_str(), length*sizeof(std::string::value_type));
  if(ret < 0) {
    std::string err = getLastError();
    throw std::runtime_error(err);
  }
  return *this;
}

Socket  & Socket::operator >> (int &value) {
  std::uint32_t len = 0;
  int ret = ::read(sfd_, &len, sizeof(len));
  if(ret < 0) {
    throw std::runtime_error(getLastError());
  }
  value = ntohl(len);
  return *this;
}

Socket  & Socket::operator >> (std::string  &value) {
  int len = 0;
  *this >> len;
  if(len < 0) {
    throw std::logic_error("string length can't be negative");
  }
  value.resize(len, '\0');

  int ret = ::read(sfd_, &value[0], len);
  if(ret < 0) {
    std::string err_string = "can't read data :" + getLastError();
    throw std::runtime_error(err_string);
  }
  return *this;
}
