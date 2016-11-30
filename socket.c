#include <errno.h>
#include <pthread.h>
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
#include "descriptor.h"
#include "readline.h"

int sck_fd;
callback_buffer_t sck_callback;
struct descriptorq sck_head = TAILQ_HEAD_INITIALIZER(sck_head);

void sck_connect(int);
void sck_parse(void *);

int
sck_init(callback_buffer_t callback)
{
  TAILQ_INIT(&sck_head);

  if (!callback)
    return -1;
  sck_callback = callback;

  int sck_fd = socket(AF_UNIX, SOCK_STREAM, 0);
  if (!sck_fd) {
    log_error("failed to created socket: %s", strerror(errno));
    return 1;
  }

  struct sockaddr_un srv_sock;
  memset(&srv_sock, 0, sizeof(srv_sock));
  srv_sock.sun_family = AF_UNIX;
  sprintf(srv_sock.sun_path, SOCKET_FMT, (int)getpid());
  unlink(srv_sock.sun_path);
  if (bind(sck_fd, (struct sockaddr*)&srv_sock, sizeof(srv_sock)) == -1) {
    log_error("failed to bind to socket: %s", strerror(errno));
    return 2;
  }

  /* FIXME: we only support a client at a time connecting to unix socket */
  if (listen(sck_fd, 1) == -1) {
    log_error("failed to listen on socket: %s", strerror(errno));
    return 2;
  }

  dsc_register(sck_fd, (callback_t*)&sck_connect);

  return 0;
}

void
sck_connect(int sck_fd)
{
  int sock_client;
  client_t client;

  if ((sock_client=accept(sck_fd, NULL, NULL)) == -1) {
    log_error("failed to accept (%d): %s", sck_fd, strerror(errno));
    return;
  }

  /* TODO: add to sck_head queue */
  client.clnt_fd = sock_client;
  pthread_create(&client.clnt_thread, NULL, sck_parse, (void*)&client);
}

void
sck_parse(void *arg)
{
  char *buf, *buf_ptr, *buf_tmp;
  size_t buf_total_sz=8, buf_ptr_sz=0, buf_free_sz=0, buf_sz=0;
  client_t *client = (client_t*)arg;

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

    buf_ptr_sz = read(client->clnt_fd, buf_ptr, buf_free_sz);
    /* FIXME: check for busy/slow device */
    if (buf_ptr_sz==0)
      break;
    if (buf_ptr_sz==-1)
      goto sck_callback_read_finished;

    buf_ptr += buf_ptr_sz;
    buf_sz += buf_ptr_sz;
  }

  sck_callback_read_finished:
  buf[buf_sz] = '\0';
  sck_callback((caddr_t*)buf);
  sck_callback_read_failed:
  close(client->clnt_fd);
  return;
}

void
sck_cleanup(void)
{
  char sck_path[256];

  sprintf(sck_path, SOCKET_FMT, (int)getpid());
  unlink(sck_path);
}
