#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <readline/readline.h>
#include <readline/history.h>
#include "log.h"
#include "completion.h"
#include "readline.h"

#define PROMPT "(config-if) "


void   callback(const char*);
char **completion(const char*, int ,int);
char  *completion_entry(const char*, int);
char  *my_generator(const char*, int);
char  *dupstr(char*);
void  *xmalloc(int);

char  *cmd[] = {"ifconfig", "shutdown", "no shutdown" ,"word", "exit"};
struct comphead comp_head = TAILQ_HEAD_INITIALIZER(comp_head);


int
main(int argc, const char **argv)
{
  struct compnode *node_ip, *node_shutdown, *node_exit;

  /* Ignore user's interruption signal */
  signal(SIGINT, SIG_IGN);

  /* Register word completions */
  comp_init(&comp_head);
  node_ip = comp_insert("ip", "IP command", &comp_head);
  comp_insert("address", "Set IP address", &node_ip->childs);
  comp_insert("forward", "Set forward IP", &node_ip->childs);
  comp_insert("default-gateway", "Set default gateway", &node_ip->childs);
  node_shutdown = comp_insert("shutdown", "Shutdown Interface", &comp_head);
  node_exit = comp_insert("exit", "Exit the node", &comp_head);

  /* Initialize readline */
  readline_init(PROMPT, callback, completion, completion_entry);

  return 0;
}

void
callback(const char *cmd)
{
  if (strcmp(cmd, "exit")==0)
    exit(0);
  if (cmd[0]==0)
    return;

  FILE *fp;
  char output[1035], command[256];

  /* Open the command for reading. */
  sprintf(command, "%s", cmd);
  printf("running %s %d\n", command, strlen(command));
}

char*
completion_entry(const char *text, int state)
{
  char *comp_text;
  static struct compnode *node;

  if (state==0) {
    if (TAILQ_EMPTY(&comp_head))
      return NULL;

    node = TAILQ_FIRST(&comp_head);
    if (strncasecmp(rl_line_buffer, "ip ", 3)==0)
      node = TAILQ_FIRST(&node->childs);
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

char**
completion(const char *text, int start, int end)
{
  char **matches;

  matches = (char **)NULL;
  //printf("[%s %s %d:%d]\n", rl_line_buffer, text, start, end);
  //if (start == 0) {
    matches = rl_completion_matches(text, &my_generator);
  //} else {
  //  rl_bind_key('\t', rl_abort);
  //}

  return (matches);
}

char*
my_generator(const char* text, int state)
{
  static int list_index, len;
  char *name;
  printf("[%s %d]\n", text, state);
  return (char*)NULL;

  if (!state) {
    list_index = 0;
    len = strlen(text);
  }

  while ((name = cmd[list_index])) {
    list_index++;

    if (strncmp(name, text, len)==0)
      return (dupstr(name));
  }

  /* If no names matched, then return NULL. */
  return ((char *)NULL);
}

char*
dupstr(char* s)
{
  char *r;

  r = (char*)malloc((strlen(s) + 1));
  strcpy(r, s);
  return r;
}

void*
xmalloc(int size)
{
  void *buf;

  buf = malloc(size);
  if (!buf) {
    fprintf(stderr, "Error: Out of memory. Exiting.'n");
    exit(1);
  }

  return buf;
}
