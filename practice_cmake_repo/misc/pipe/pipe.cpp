#include "pipe.hpp"
#include <stdexcept>
#include <vector>

#include <iostream>

#ifdef __linux__
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#endif

Pipe::Pipe(MODE mode)
  :mode_(mode)
#ifdef WIN32
   ,input_(nullptr), output_(nullptr)
#endif
{}

Pipe::Pipe(MODE mode, const std::string  &process)
  :mode_(mode)
#ifdef WIN32
  , input_(nullptr), output_(nullptr)
#endif
{
  if (!open(process)) {
    throw std::runtime_error("failed to open pipe");
  }
}

Pipe::~Pipe()
{
#ifdef WIN32  
  if (output_) {
    ::CloseHandle(output_);
  }
#else
  void (*sigcld_handler)(int);
  pid_t pid = 0;
  sigcld_handler = ::signal(SIGCHLD, SIG_DFL);
  if(pid_) {
    do {
      pid = ::waitpid(pid_, 0, 0);
    } while(pid == -1);
    pid_ = 0;
  }
  ::signal(SIGCHLD, sigcld_handler); 
#endif
}

bool Pipe::open(const std::string  &process, const std::string  &args) {
#ifdef WIN32
  HANDLE childStdoutRd = nullptr;
  HANDLE childStdoutWr = nullptr;
  HANDLE childStdinRd = nullptr;
  HANDLE childStdinWr = nullptr;

  SECURITY_ATTRIBUTES saAttr;

  saAttr.nLength = sizeof(SECURITY_ATTRIBUTES);
  saAttr.bInheritHandle = TRUE;
  saAttr.lpSecurityDescriptor = NULL;

  if (!CreatePipe(&childStdoutRd, &childStdoutWr, &saAttr, 0)) {
    throw std::runtime_error(getError());
  }
  if (!SetHandleInformation(childStdoutRd, HANDLE_FLAG_INHERIT, 0)) {
    throw std::runtime_error(getError());
  }
  if (!CreatePipe(&childStdinRd, &childStdinWr, &saAttr, 0)) {
    throw std::runtime_error(getError());
  }
  if (!SetHandleInformation(childStdinWr, HANDLE_FLAG_INHERIT, 0)) {
    throw std::runtime_error(getError());
  }

  STARTUPINFOA si;
  PROCESS_INFORMATION pi;

  ZeroMemory(&si, sizeof(STARTUPINFO));
  si.cb = sizeof(STARTUPINFO);
  si.hStdError = childStdoutWr;
  si.hStdOutput = childStdoutWr;
  si.hStdInput = childStdinRd;
  si.dwFlags |= STARTF_USESTDHANDLES;

  std::string cmd = process;

  if (!CreateProcessA(cmd.c_str(), (LPSTR)args.c_str(), NULL, NULL, TRUE, 0, NULL, NULL, &si, &pi)) {
    throw std::runtime_error(getError());
  }

  //explicitly close the our end of the child processes stdout pipe;
  //without doing so we end up in a deadlock sitation when the child closes their end of the pipe
  CloseHandle(childStdoutWr);

  CloseHandle(pi.hProcess);
  CloseHandle(pi.hThread);

  if (mode_ == MODE::READ_ONLY || mode_ == MODE::READ_WRITE) {
    input_ = childStdoutRd;
  }

  if (mode_ == MODE::WRITE_ONLY || mode_ == MODE::READ_WRITE) {
    output_ = childStdinWr;
  }
#else
  int ret = pipe(pipefd);
  if(ret != 0) {
     char  *errMsg = strerror(ret);
     std::string str(static_cast<const char*>(errMsg));
     std::cout << str << std::endl;
  }

  // how to close all file descriptors
  //http://stackoverflow.com/questions/899038/getting-the-highest-allocated-file-descriptor/918469#918469

  pid_ = fork();
  switch(pid_) {
    case -1:
    {
      char *errMsg = strerror(errno);
      std::string str(static_cast<const char *>(errMsg));
      std::cout << str << std::endl;
    }break;
   //  pipefd[0] refers to the read end of the pipe.
   //  pipefd[1] refers to the write end of the pipe.
   // 0 - stdin, 1 - stdout, 2- stderror 
    case 0:
    {
       int maxfd=sysconf(_SC_OPEN_MAX);
       for(int fd=3; fd<maxfd; fd++) {
         close(fd);
       }

      //this child
      if (mode_ == MODE::WRITE_ONLY) {
        close(1);
        dup2(pipefd[1], 1);
      }

      if (mode_ == MODE::READ_ONLY) {
        close(0);
        dup2(pipefd[0], 0);
      }
      close(pipefd[0]);
      close(pipefd[1]);

      std::string procWithArgs = process + std::string(" ") + args;
//http://stackoverflow.com/questions/14301407/how-does-execvp-run-a-command
      //int ret = execlp("/bin/sh", "sh", "-c", quotedProcess.c_str(), (char *)0);
      int ret = execl("/bin/sh","sh", "-c", procWithArgs.c_str(), (char*)0);
      if(ret == -1) {
        char *errMsg = strerror(errno);
        std::string str(static_cast<const char *>(errMsg));
        std::cout << "execl = " << str << std::endl;
      }
      exit(1);
    }break;
    default:
    {
      //this is parent
      if (mode_ == MODE::WRITE_ONLY) {
        close(pipefd[0]);
      }
      if (mode_ == MODE::READ_ONLY) {
        close(pipefd[1]);
      }
    };
  }
#endif
  return 0;
}

bool Pipe::open(const std::string  &process) {
#ifdef WIN32
  HANDLE childStdoutRd = nullptr;
  HANDLE childStdoutWr = nullptr;
  HANDLE childStdinRd = nullptr;
  HANDLE childStdinWr = nullptr;

  SECURITY_ATTRIBUTES saAttr;

  saAttr.nLength = sizeof(SECURITY_ATTRIBUTES);
  saAttr.bInheritHandle = TRUE;
  saAttr.lpSecurityDescriptor = NULL;

  if (!CreatePipe(&childStdoutRd, &childStdoutWr, &saAttr, 0)) {
    throw std::runtime_error(getError());
  }
  if (!SetHandleInformation(childStdoutRd, HANDLE_FLAG_INHERIT, 0)) {
    throw std::runtime_error(getError());
  }
  if (!CreatePipe(&childStdinRd, &childStdinWr, &saAttr, 0)) {
    throw std::runtime_error(getError());
  }
  if (!SetHandleInformation(childStdinWr, HANDLE_FLAG_INHERIT, 0)) {
    throw std::runtime_error(getError());
  }

  STARTUPINFOA si;
  PROCESS_INFORMATION pi;

  ZeroMemory(&si, sizeof(STARTUPINFO));
  si.cb = sizeof(STARTUPINFO);
  si.hStdError = childStdoutWr;
  si.hStdOutput = childStdoutWr;
  si.hStdInput = childStdinRd;
  si.dwFlags |= STARTF_USESTDHANDLES;
  
  std::string cmd = process;

  if (!CreateProcessA(cmd.c_str(), NULL, NULL, NULL, TRUE, 0, NULL, NULL, &si, &pi)) {
    throw std::runtime_error(getError());
  }

  //explicitly close the our end of the child processes stdout pipe;
  //without doing so we end up in a deadlock sitation when the child closes their end of the pipe
  CloseHandle(childStdoutWr);

  CloseHandle(pi.hProcess);
  CloseHandle(pi.hThread);

  if (mode_ == MODE::READ_ONLY || mode_ == MODE::READ_WRITE) {
    input_ = childStdoutRd;
  }

  if (mode_ == MODE::WRITE_ONLY || mode_ == MODE::READ_WRITE) {
    output_ = childStdinWr;
  }
#else
  int ret = pipe(pipefd);
  if(ret != 0) {
     char  *errMsg = strerror(ret);
     std::string str(static_cast<const char*>(errMsg));
     std::cout << str << std::endl;
  }

  // how to close all file descriptors
  //http://stackoverflow.com/questions/899038/getting-the-highest-allocated-file-descriptor/918469#918469

  pid_ = fork();
  switch(pid_) {
    case -1:
    {
      char *errMsg = strerror(errno);
      std::string str(static_cast<const char *>(errMsg));
      std::cout << str << std::endl;
    }break;
   //  pipefd[0] refers to the read end of the pipe.
   //  pipefd[1] refers to the write end of the pipe.
   // 0 - stdin, 1 - stdout, 2- stderror 
    case 0:
    {
       int maxfd=sysconf(_SC_OPEN_MAX);
       for(int fd=3; fd<maxfd; fd++) {
         close(fd);
       }

      //this child
      if (mode_ == MODE::WRITE_ONLY) {
        //close(pipefd[1]);
        close(0);
        dup2(pipefd[0], 0);
      }

      if (mode_ == MODE::READ_ONLY) {
        //close(pipefd[0]);
        close(1);
        dup2(pipefd[1], 1);
      }
      close(pipefd[0]);
      close(pipefd[1]);

      int ret = execl("/bin/sh", "sh", "-c", process.c_str(), (char *)0);
      if(ret == -1) {
        char *errMsg = strerror(errno);
        std::string str(static_cast<const char *>(errMsg));
        std::cout << "execl = " << str << std::endl;
      }
      exit(1);
    }break;
    default:
    {
      //this is parent
      if (mode_ == MODE::WRITE_ONLY) {
        close(pipefd[0]);
      }
      if (mode_ == MODE::READ_ONLY) {
        close(pipefd[1]);
      }
    };
  }
#endif
  return true;
}

void Pipe::write(const std::string  &data) {
#ifdef WIN32
  if (output_) {
    DWORD writtenBites = 0;
    BOOL res = WriteFile(output_, data.c_str(), static_cast<DWORD>(data.length()), &writtenBites, 0);
    if (!res || &writtenBites == 0) {
      throw std::runtime_error(getError());
    }
  } else {
    throw std::logic_error("Logical error. you can't write to pipe in MODE::READ_ONLY");
  }
#else
  std::string terminated_data  = data;
  terminated_data += '\n';
  std::size_t ret = ::write(pipefd[1], static_cast<const void*>(terminated_data.c_str()), terminated_data.length());
#endif
}

std::string Pipe::read(std::size_t  length) {
#ifdef WIN32
  if (input_) {
    std::vector<char> data(length);
    DWORD readBytes = 0;
    BOOL res = ReadFile(input_, &data[0], static_cast<DWORD>(data.size()), &readBytes, 0);
    if (!res || readBytes == 0) {
      throw std::runtime_error(getError());
    }
    return std::string(data.begin(), data.end());
  } else {
    throw std::logic_error("Logical error. you can't read from pipe in MODE::WRITE_ONLY");
  }
  return std::string();
#else
  std::vector<char> data(length);
  ::read(pipefd[0], &data[0], data.size());
  return std::string(data.begin(), data.end());
#endif
}

#ifdef WIN32
std::string Pipe::getError() const {
  DWORD errorMessageID = GetLastError();
  if (errorMessageID == 0) {
    return std::string();
  }

  LPSTR msgBuffer = nullptr;
  std::size_t size = FormatMessageA(FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS,
    NULL, errorMessageID, NULL, (LPSTR)&msgBuffer, 0, NULL);

  std::string msg(static_cast<const char*>(msgBuffer), size);

  LocalFree(msgBuffer);
  return msg;
}
#else
std::string Pipe::getError() const {
  return std::string();
}
#endif