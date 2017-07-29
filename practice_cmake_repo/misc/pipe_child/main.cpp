#include <iostream>
#include <string>
#include <fstream>

auto main(int argc, char *argv[]) -> int {

  std::string data;
  std::ofstream ofs("logfile.log");
  while (std::getline(std::cin, data)) {
    if (ofs) {
      ofs << data << std::endl;
      ofs.flush();
    } else {
      data += " ofs fails";
    }
    if (data == "quit") {
      break;
    }
    std::cout << data << std::endl;
  }
  return 0;
}


#ifdef WIN32
#include <windows.h>

int main(void)
{
  CHAR chBuf[BUFSIZE];
  DWORD dwRead, dwWritten;
  HANDLE hStdin, hStdout;
  BOOL bSuccess;

  hStdout = GetStdHandle(STD_OUTPUT_HANDLE);
  hStdin = GetStdHandle(STD_INPUT_HANDLE);
  if (
    (hStdout == INVALID_HANDLE_VALUE) ||
    (hStdin == INVALID_HANDLE_VALUE)
    )
    ExitProcess(1);

  // Send something to this process's stdout using printf.
  printf("\n ** This is a message from the child process. ** \n");

  // This simple algorithm uses the existence of the pipes to control execution.
  // It relies on the pipe buffers to ensure that no data is lost.
  // Larger applications would use more advanced process control.

  for (;;)
  {
    // Read from standard input and stop on error or no data.
    bSuccess = ReadFile(hStdin, chBuf, BUFSIZE, &dwRead, NULL);

    const char quitCmd[] = "quit";
    if (!std::strncmp(chBuf, quitCmd, 4)) {
      break;
    }

    if (!bSuccess || dwRead == 0)
      break;

    // Write to standard output and stop on error.
    bSuccess = WriteFile(hStdout, chBuf, dwRead, &dwWritten, NULL);

    if (!bSuccess)
      break;
  }
  return 0;
}
#else
/*
#include <cstring>
#define BUFSIZE 4096
#include <stdio.h>

auto main(int argc, char*argv[]) -> int {
  char buff[BUFSIZE];
  std::ofstream ofs("logfile.log");
  for(;;) {
    std::size_t bytes = fread(static_cast<void*>(buff), sizeof(char), sizeof(buff)/sizeof(char),stdin);
    if(ofs && bytes) {
      ofs << buff << std::endl;
    }
    const char quitCmd[] = "quit";
    if(!std::strncmp(buff, quitCmd, 4)) {
      break;
    }
    int len = strlen(buff);
    fwrite(static_cast<const void*>(buff), sizeof(char), len, stdout);
  }
  return 0;
}*/
#endif
