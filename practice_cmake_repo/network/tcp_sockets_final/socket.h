#ifndef SOCKET_H
#define SOCKET_H

#include <string>
#include <memory>

class Socket {
public:
  Socket(int sock);
  Socket(const std::string  &addr, unsigned short port);
  ~Socket();

  Socket(const Socket  &socket) = default;
  Socket & operator = (const Socket  &socket) = default;

  void connect() const;
  void connect(const std::string  &addr, unsigned short port);

  void listen() const;
  std::shared_ptr<Socket> accept() const;

  void close();
  bool is_opened() const;

  Socket  & operator << (int value);
  Socket  & operator << (const std::string  &value);
  Socket  & operator >> (int &value);
  Socket  & operator >> (std::string  &value);

private:
  std::string addr_;
  unsigned short port_;
  int sfd_;
};

#endif
