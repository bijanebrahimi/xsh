#include <stdio.h>
#include <sys/queue.h>
#include "completion.h"

int
comp_init(struct comphead *comp_head)
{
  TAILQ_INIT(comp_head);
  return 0;
}


struct compnode *
comp_insert(const char *name, const char *description, struct comphead *head)
{
  struct compnode *node;
  node = malloc(sizeof(struct compnode));
  snprintf(node->name, sizeof(node->name), name);
  snprintf(node->description, sizeof(node->description), description);
  TAILQ_INIT(&node->childs);

  if (TAILQ_EMPTY(head))
    TAILQ_INSERT_HEAD(head, node, next);
  else
    TAILQ_INSERT_TAIL(head, node, next);

  return node;
}
