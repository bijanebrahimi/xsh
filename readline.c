#include <stdio.h>
#include <sys/queue.h>
#include <stdlib.h>
#include <readline/readline.h>
#include <readline/history.h>
#include "readline.h"


int
rln_init(const char *prompt, void (*callback)(const char*),
              char (*completion)(const char*,int ,int))
{
  char *buf;

  /* gnu readline initialization */
  rl_attempted_completion_function = completion;

  /* maximum number to show without confirms */
  rl_completion_query_items = 80;

  /* default break word characters */
  //rl_basic_word_break_characters = " \t\n\"\\'`@$><=;|&{(";

  while ((buf = readline(prompt))!=NULL) {
    rl_bind_key('\t', rl_complete);

    /* Ignoring empty commands */
    if (buf[0]==0)
      continue;

    /* Ignore adding commands to history if spaced */
    if (buf[0]!=' ')
      add_history(buf);

    callback(buf);
  }
  free(buf);

  return 0;
}

struct compnode *
rln_completion_insert(const char *name, const char *description, struct comphead *head)
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
rln_completion_find_name(const char *name, struct comphead *head)
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
