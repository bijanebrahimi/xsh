#ifndef _THREAD_H_
#define _THREAD_H_

#include "types.h"

TAILQ_HEAD(clientq, client);

typedef struct client {
  int clnt_fd;
  callback_t *clnt_func;

  TAILQ_ENTRY(client) siblings;
} client_t;

extern int srv_init();
extern int srv_loop();
extern int srv_register(int, callback_t*);

#endif
