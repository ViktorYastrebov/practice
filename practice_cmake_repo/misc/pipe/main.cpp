#include <iostream>
#include "pipe.hpp"

void child_process_test(const std::string  &child_process) {
  Pipe pipe(Pipe::READ_WRITE);
  pipe.open(child_process);

  std::string line;
  while (std::getline(std::cin, line)) {
    pipe.write(line);
    if (line == "quit") {
      break;
    }
    std::string rdData = pipe.read(1024);
    std::cout << "read data : " << rdData << std::endl; 
  }
}

void ping_test(const std::string  &ping, const std::string args) {
  Pipe pipe(Pipe::READ_ONLY);
  pipe.open(ping, args);
  std::string ping_res = pipe.read(1024);
  std::cout << ping_res << std::endl;
}


auto main(int argc, char *argv[]) -> int {

#ifdef WIN32
  std::string child_path("d:\\programming_workspace\\c++\\practice_cmake_repo\\Debug\\bin\\Debug\\pipe_child_example.exe");
  child_process_test(child_path);

  std::string ping_path("c:\\Windows\\system32\\ping.exe");
  std::string ip("127.0.0.1");
  ping_test(ping_path, ip);
#elif defined __linux__
  std::string child_path("./pipe_child_example");
  child_process_test(child_path);

  std::string ping("ping");
  std::string ip("-c 1 127.0.0.1");
  ping_test(ping, ip);
#endif
  return 0;
}

//https://www.cs.rutgers.edu/~pxk/416/notes/c-tutorials/pipe.html

/*


#include <sys/types.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>

// Read characters from the pipe and echo them to stdout.

void
read_from_pipe (int file)
{
  FILE *stream;
  int c;
  stream = fdopen (file, "r");
  while ((c = fgetc (stream)) != EOF)
    putchar (c);
  fclose (stream);
}

// Write some random text to the pipe.

void
write_to_pipe (int file)
{
  FILE *stream;
  stream = fdopen (file, "w");
  fprintf (stream, "hello, world!\n");
  fprintf (stream, "goodbye, world!\n");
  fclose (stream);
}

int
main (void)
{
  pid_t pid;
  int mypipe[2];

  // Create the pipe.
  if (pipe (mypipe))
    {
      fprintf (stderr, "Pipe failed.\n");
      return EXIT_FAILURE;
    }

  // Create the child process.
  pid = fork ();
  if (pid == (pid_t) 0)
    {
      // This is the child process. Close other end first.
      close (mypipe[1]);
      read_from_pipe (mypipe[0]);
      return EXIT_SUCCESS;
    }
  else if (pid < (pid_t) 0)
    {
      //The fork failed.
      fprintf (stderr, "Fork failed.\n");
      return EXIT_FAILURE;
    }
  else
    {
      // This is the parent process. Close other end first.
      close (mypipe[0]);
      write_to_pipe (mypipe[1]);
      return EXIT_SUCCESS;
    }
}
*/
/*
#include <unistd.h>
#include <stdio.h>

#define MSGSIZE 32

char *msg1 = "hello #1";
char *msg2 = "hello #2";
char *msg3 = "hello #3";

main()
{  char inbuf[MSGSIZE];

   int p[2], j, pid;

   // open pipe

   if(pipe(p) == -1)
   {    perror("pipe call error");
        exit(1);
   }

   switch(pid = fork()){
   case -1: perror("error: fork call");
            exit(2);

   case 0:  // if child then write down pipe
         close(p[0]);  // first close the read end of the pipe
	 write(p[1], msg1, MSGSIZE);
	 write(p[1], msg2, MSGSIZE);
	 write(p[1], msg3, MSGSIZE);
         break;
   default:   // parent reads pipe
         close(p[1]);  // first close the write end of the pipe
	 for(j=0; j<3; j++)
	 {   read(p[0], inbuf, MSGSIZE);
	     printf("Parent: %s\n", inbuf);
	 }
         wait(NULL);
   }

   exit(0);
}*/