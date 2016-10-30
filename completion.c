#include <stdio.h>
#include <sys/queue.h>
#include "completion.h"

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

struct compnode *
comp_find_name(const char *name, struct comphead *head)
{
  struct compnode *node, *found_node=NULL;
  if (TAILQ_EMPTY(head))
    return NULL;

  node = TAILQ_FIRST(head);
  TAILQ_FOREACH(node, head, next) {
    if (strncasecmp(name, node->name, strlen(name))==0) {
      if (!found_node)
        found_node = node;
      else
        return NULL;
    }
  }
  return found_node;
}
