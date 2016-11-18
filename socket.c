#include <errno.h>
#include <unistd.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/un.h>
#include <sys/socket.h>
#include "log.h"
#include "socket.h"
#include "readline.h"

int
sck_init(void (*callback)(const char*))
{
  int fd = socket(AF_UNIX, SOCK_STREAM, 0);
  if (!fd) {
    log_print(LOG_ERR, "failed to created socket: %s", strerror(errno));
    return 1;
  }

  struct sockaddr_un srv_sock;
  memset(&srv_sock, 0, sizeof(srv_sock));
  srv_sock.sun_family = AF_UNIX;
  sprintf(srv_sock.sun_path, SOCKET_FMT, (int)getpid());
  if (bind(fd, (struct sockaddr*)&srv_sock, sizeof(srv_sock)) == -1) {
    log_print(LOG_ERR, "failed to bind to socket: %s", strerror(errno));
    return 2;
  }

  if (listen(fd, 5) == -1) {
    log_print(LOG_ERR, "failed to listen to socket: %s", strerror(errno));
    return 3;
  }

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

void
sck_cleanup(int signo) {
  pid_t pid;
  char buff[256];

  sprintf(buff, SOCKET_FMT, (int)getpid());
  unlink(buff);
  exit(0);
}
