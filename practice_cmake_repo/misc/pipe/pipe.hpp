#pragma once

#ifdef WIN32
  #include <windows.h>
#else
  #include <sys/types.h>
#endif
#include <string>


class Pipe {
public:

  enum MODE {
    READ_ONLY,
    WRITE_ONLY,
    READ_WRITE,
  };
  Pipe(MODE mode);
  Pipe(MODE mode, const std::string  &process);
  virtual ~Pipe();

  bool open(const std::string  &process);
  bool open(const std::string  &process, const std::string  &args);

  void write(const std::string  &data);
  std::string read(std::size_t  length);

private:
  std::string getError() const;
private:
  MODE mode_;
#ifdef WIN32
  HANDLE input_;
  HANDLE output_;
#else
  pid_t pid_;
  int pipefd[2];
#endif
};