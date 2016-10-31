#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <sys/queue.h>
#include <readline/readline.h>
#include <readline/history.h>
#include "log.h"
#include "readline.h"

#define PROMPT "(config-if) "
void   callback(const char*);
char  *completion_entry(const char*, int);
char **completion(const char*, int, int);
int    help(int, int);

struct comphead comp_head = TAILQ_HEAD_INITIALIZER(comp_head);
struct comphead *comp_curr;

int
main(int argc, const char **argv)
{
  struct compnode *node, *node_tmp;

  /* Ignore user's interruption signal */
  TAILQ_INIT(&comp_head);
  /* TODO: ignore user's C^Z C^D */
  signal(SIGINT, SIG_IGN);

  /* Register word completions */
  /*
  node = rln_completion_insert("ip", "IP command", &comp_head);
  rln_completion_insert("address", "Set IP address", &node->childs);
  rln_completion_insert("forward", "Set forward IP", &node->childs);
  rln_completion_insert("default-gateway", "Set default gateway", &node->childs);
  node_tmp = rln_completion_insert("dhcp", "Set default gateway", &node->childs);
  rln_completion_insert("server", "Set DHCP server", &node_tmp->childs);
  node = rln_completion_insert("shutdown", "Shutdown Interface", &comp_head);
  node = rln_completion_insert("exit", "Exit the node", &comp_head);
  */

  rln_completion("ip address <ip>/<netmask> [secondary]", &comp_head);
  rln_completion("ip address dhcp", &comp_head);
  rln_completion("ip default-gateway <ip>", &comp_head);
  rln_completion("shutdown", &comp_head);
  rln_completion("exit", &comp_head);

  /* Initialize readline */
  rln_init(PROMPT, callback, completion, help);

  return 0;
}

void
callback(const char *cmd)
{
  if (strcmp(cmd, "exit")==0)
    exit(0);

  /* Open the command for reading. */
  // rl_message("running %s %d\n", cmd, strlen(cmd));
  rl_message("\n");
}

char**
completion(const char *text, int start, int end)
{
  struct compnode *node;
  struct comphead *head;
  char **matches, *comp_text, *comp_text_ptr, *comp_token;

  head = &comp_head;
  matches = (char **)NULL;
  comp_text_ptr = comp_text = strndup(rl_line_buffer, start);
  while ((comp_token=strsep(&comp_text, " "))!=NULL) {
    /* Skip the seperator itself */
    if (comp_token[0]=='\0')
      continue;

    node = rln_completion_find_name(comp_token, head);
    if (node) {
      head = &node->head;
    } else {
      return (matches);
    }
  }

  if (!TAILQ_EMPTY(head)) {
    comp_curr = head;
    matches = rl_completion_matches(text, &completion_entry);
  }

  free(comp_text_ptr);
  return (matches);
}

int
help(int _unused, int __unused)
{
  int start;
  struct compnode *node;
  struct comphead *head;
  char **matches, *comp_text, *comp_text_ptr, *comp_token;


  head = &comp_head;
  matches = (char **)NULL;
  comp_text_ptr = comp_text = strndup(rl_line_buffer, rl_point);
  while ((comp_token=strsep(&comp_text, " "))!=NULL) {
    /* Skip the seperator itself */
    if (comp_token[0]=='\0')
      continue;

    node = rln_completion_find_name(comp_token, head);
    if (node) {
      head = &node->head;
    } else {
      return (matches);
    }
  }

  if (!TAILQ_EMPTY(head)) {
    printf("\n");
    TAILQ_FOREACH(node, head, next) {
      printf("%s\n", node->syntax);
    }
    rl_forced_update_display();
  }

  free(comp_text_ptr);
  return 0;
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
    if (strncasecmp(text, node->text, strlen(text))==0) {
      comp_text = strdup(node->text);
      node = TAILQ_NEXT(node, next);
      return comp_text;
    }
    node = TAILQ_NEXT(node, next);
  }

  return NULL;
}
