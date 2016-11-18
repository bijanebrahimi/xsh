#include <errno.h>
#include <unistd.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/un.h>
#include <sys/socket.h>
#include <sys/queue.h>
#include "types.h"
#include "log.h"
#include "socket.h"
#include "server.h"
#include "readline.h"

int sock_un;
void sck_callback(int);

int
sck_init(void)
{
  int sock_un = socket(AF_UNIX, SOCK_STREAM, 0);
  if (!sock_un) {
    log_error("failed to created socket: %s", strerror(errno));
    return 1;
  }

  struct sockaddr_un srv_sock;
  memset(&srv_sock, 0, sizeof(srv_sock));
  srv_sock.sun_family = AF_UNIX;
  sprintf(srv_sock.sun_path, SOCKET_FMT, (int)getpid());
  unlink(srv_sock.sun_path);
  if (bind(sock_un, (struct sockaddr*)&srv_sock, sizeof(srv_sock)) == -1) {
    log_error("failed to bind to socket: %s", strerror(errno));
    return 2;
  }

  /* FIXME: we only support a client at a time connecting to unix socket */
  if (listen(sock_un, 1) == -1) {
    log_error("failed to listen on socket: %s", strerror(errno));
    return 2;
  }

  srv_register(sock_un, (callback_t*)&sck_callback);

  return 0;
}

void
sck_callback(int sock_un)
{
  int sock_client;
  char *buf, *buf_ptr, *buf_tmp;
  size_t buf_total_sz=8, buf_ptr_sz=0, buf_free_sz=0, buf_sz=0;

  if ((sock_client=accept(sock_un, NULL, NULL)) == -1) {
    log_error("failed to accept (%d): %s", sock_un, strerror(errno));
    return;
  }

  /* TODO: should communicate using a defined command struct header. */
  buf = malloc(buf_total_sz);
  buf_ptr = buf;
  if (!buf) {
    log_error("failed to allocated memory: %s", strerror(errno));
    goto sck_callback_read_failed;
  }

  while (true) {
    buf_free_sz = buf + buf_total_sz - buf_ptr - 1;
    if (buf_free_sz<=0) {
      buf_total_sz *= 2;
      buf_tmp = realloc(buf, buf_total_sz);

      /* Returning current buffer in case no more memory is available */
      if (!buf_tmp)
        break;

      buf = buf_tmp;
      buf_ptr = buf + buf_sz;
      buf_free_sz = buf + buf_total_sz - buf_ptr - 1;
    }

    buf_ptr_sz = read(sock_client, buf_ptr, buf_free_sz);
    /* FIXME: check for busy/slow device */
    if (buf_ptr_sz==0)
      break;
    if (buf_ptr_sz==-1)
      goto sck_callback_read_finished;

    buf_ptr += buf_ptr_sz;
    buf_sz += buf_ptr_sz;
  }

  buf[buf_sz] = '\0';
  log_info("received: %s", buf);
  sck_callback_read_finished:
  free(buf);
  sck_callback_read_failed:
  close(sock_client);
  return;
}

void
sck_cleanup(void)
{
  char sck_path[256];

  sprintf(sck_path, SOCKET_FMT, (int)getpid());
  unlink(sck_path);
}
