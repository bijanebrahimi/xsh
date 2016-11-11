#include <sys/socket.h>
#include <sys/un.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <signal.h>
#include "socket.h"
#include "readline.h"

void handle_term(int signo) {
  char buff[256];
  printf("SIGNAL %d\n", signo);
  sprintf(buff, SOCKET_FMT, 0);
  unlink(buff);
  exit(0);
}


int
sck_init(void (*callback)(const char*), struct complhead* head)
{
  int fd = socket(AF_UNIX, SOCK_STREAM, 0);
  if (!fd)
    return 1;

  signal(SIGTERM, handle_term);
  signal(SIGINT, handle_term);

  struct sockaddr_un addr;
  memset(&addr, 0, sizeof(addr));
  addr.sun_family = AF_UNIX;
  sprintf(addr.sun_path, SOCKET_FMT, 0);
  if (bind(fd, (struct sockaddr*)&addr, sizeof(addr)) == -1)
    return 2;

  if (listen(fd, 5) == -1)
    return 3;

  char buff[1024];
  int clnt, bytes;
  while (1) {
    if ((clnt=accept(fd, NULL, NULL)) == -1)
      continue;

    dup2(clnt, STDOUT_FILENO);
    dup2(clnt, STDERR_FILENO);

    while ((bytes=read(clnt, buff, sizeof(buff)))) {
      if (bytes == -1)
        return 4;
      else if (bytes == 0) {
        close(clnt);
        break;
      }

      char *line = strchr(buff, '\n');
      if (line)
        *line='\0';
      callback(buff);
    }
  }

  return 0;
}
