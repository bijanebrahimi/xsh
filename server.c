#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/queue.h>
#include "types.h"
#include "server.h"

struct clientq head = TAILQ_HEAD_INITIALIZER(head);
int srv_append(client_t*);
int srv_remove(client_t*);

int
srv_init(void)
{
  TAILQ_INIT(&head);
}

int
srv_loop(void)
{
  int ready_fds, max_fd=0;
  fd_set read_set;
  client_t *thread;

  /* It's a forever loop */
  while (1) {
    /* Initializing read_set */
    FD_ZERO(&read_set);
    TAILQ_FOREACH(thread, &head, siblings) {
      if (thread->clnt_fd<0)
        continue;

      FD_SET(thread->clnt_fd, &read_set);
      max_fd = max(thread->clnt_fd, max_fd);
    }

    /* Check if there's ready descriptors */
    ready_fds = select(max_fd+1, &read_set, NULL, NULL, NULL);
    if (ready_fds == 0)
      continue;
    if (ready_fds == -1){
      if (errno==EINTR)
        continue;

      /* FIXME: need better handling the error situation */
      exit(1);
    }

    /*
     * Traverse the thread list to find matching callbacks for each
     * ready descriptor
     */
    TAILQ_FOREACH(thread, &head, siblings) {
      if (FD_ISSET(thread->clnt_fd, &read_set)) {
        /* Execute the callback function */
        thread->clnt_func();
        FD_CLR(thread->clnt_fd, &read_set);
      }
    }
  }

  /* FIXME: this should never reach. Implement a graceful shutdown */
}

int
srv_register(int clnt_fd, callback_t *clnt_func)
{
  if (clnt_fd<0 || clnt_func==NULL)
    return -1;

  client_t *thread = malloc(sizeof(client_t));
  if (!thread )
    return -1;

  thread->clnt_fd = clnt_fd;
  thread->clnt_func = clnt_func;

  return srv_append(thread);
}

int
srv_append(client_t *thread)
{
  if (TAILQ_EMPTY(&head))
    TAILQ_INSERT_HEAD(&head, thread, siblings);
  else
    TAILQ_INSERT_TAIL(&head, thread, siblings);

  return 0;
}

int
srv_unregister(int socket, callback_t func)
{

}

int
srv_remove(client_t *thread)
{
  TAILQ_REMOVE(&head, thread, siblings);

  return 0;
}

int
srv_notify(int socket)
{

}
