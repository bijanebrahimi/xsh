#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/queue.h>
#include "types.h"
#include "descriptor.h"

struct descriptor_queue head = TAILQ_HEAD_INITIALIZER(head);
int dsc_append(descriptor_t*);
int dsc_remove(descriptor_t*);

int
dsc_init(void)
{
  TAILQ_INIT(&head);
}

int
dsc_loop(void)
{
  int ready_fds, max_fd=0;
  fd_set read_set;
  descriptor_t *thread;

  /* It's a forever loop */
  while (1) {
    /* Initializing read_set */
    FD_ZERO(&read_set);
    TAILQ_FOREACH(thread, &head, siblings) {
      if (thread->dsc_no<0)
        continue;

      FD_SET(thread->dsc_no, &read_set);
      max_fd = max(thread->dsc_no, max_fd);
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
      if (FD_ISSET(thread->dsc_no, &read_set)) {
        /* Execute the callback function */
        thread->dsc_func(thread->dsc_no);
        FD_CLR(thread->dsc_no, &read_set);
      }
    }
  }

  /* FIXME: this should never reach. Implement a graceful shutdown */
}

int
dsc_register(int dsc_no, callback_t *dsc_func)
{
  if (dsc_no<0 || dsc_func==NULL)
    return -1;

  descriptor_t *thread = malloc(sizeof(descriptor_t));
  if (!thread)
    return -1;

  thread->dsc_no = dsc_no;
  thread->dsc_func = dsc_func;

  return dsc_append(thread);
}

int
dsc_append(descriptor_t *thread)
{
  if (TAILQ_EMPTY(&head))
    TAILQ_INSERT_HEAD(&head, thread, siblings);
  else
    TAILQ_INSERT_TAIL(&head, thread, siblings);

  return 0;
}

int
dsc_unregister(int socket, callback_t func)
{

}

int
dsc_remove(descriptor_t *thread)
{
  TAILQ_REMOVE(&head, thread, siblings);

  return 0;
}

int
dsc_notify(int socket)
{

}
