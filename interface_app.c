#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <sys/queue.h>
#include <readline/readline.h>
#include <readline/history.h>
#include "log.h"
#include "completion.h"
#include "readline.h"
#define PROMPT "(config-if) "


void   callback(const char*);
char  *completion_entry(const char*, int);
char **completion(const char*, int, int);

struct comphead comp_head = TAILQ_HEAD_INITIALIZER(comp_head);
struct comphead *comp_curr;

int
main(int argc, const char **argv)
{
  struct compnode *node, *node_tmp;

  /* Ignore user's interruption signal */
  TAILQ_INIT(&comp_head);
  signal(SIGINT, SIG_IGN);

  /* Register word completions */
  node = comp_insert("ip", "IP command", &comp_head);
  comp_insert("address", "Set IP address", &node->childs);
  comp_insert("forward", "Set forward IP", &node->childs);
  comp_insert("default-gateway", "Set default gateway", &node->childs);
  node_tmp = comp_insert("dhcp", "Set default gateway", &node->childs);
  comp_insert("server", "Set DHCP server", &node_tmp->childs);

  node = comp_insert("shutdown", "Shutdown Interface", &comp_head);
  node = comp_insert("exit", "Exit the node", &comp_head);

  /* Initialize readline */
  readline_init(PROMPT, callback, completion);

  return 0;
}

void
callback(const char *cmd)
{
  if (strcmp(cmd, "exit")==0)
    exit(0);

  /* Open the command for reading. */
  printf("running %s %d\n", cmd, strlen(cmd));
}

char**
completion(const char *text, int start, int end)
{
  struct compnode *node;
  struct comphead *head;
  char **matches, *comp_text, *comp_token;
  // printf("[%s][%s][%d:%d]", rl_line_buffer, text, start, end);

  head = &comp_head;
  matches = (char **)NULL;
  comp_text = strndup(rl_line_buffer, start);
  while ((comp_token=strsep(&comp_text, " "))!=NULL) {
    /* Skip the seperator itself */
    if (comp_token[0]=='\0')
      continue;

    node = comp_find_name(comp_token, head);
    if (node)
      head = &node->childs;
    else
      return (matches);
  }

  if (!TAILQ_EMPTY(head)) {
    comp_curr = head;
    matches = rl_completion_matches(text, &completion_entry);
  }

  return (matches);
}


char*
completion_entry(const char *text, int state)
{
  char *comp_text, *line_buffer;
  static struct compnode *node;

  if (state==0) {
    node = TAILQ_FIRST(comp_curr);
  }

  while (node) {
    if (strncasecmp(text, node->name, strlen(text))==0) {
      comp_text = strdup(node->name);
      node = TAILQ_NEXT(node, next);
      return comp_text;
    }
    node = TAILQ_NEXT(node, next);
  }

  return NULL;
}
