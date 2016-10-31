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

struct comphead comp_head = TAILQ_HEAD_INITIALIZER(comp_head);

int
main(int argc, const char **argv)
{
  struct compnode *node, *node_tmp;

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
  rln_init(PROMPT, callback, &comp_head);

  return 0;
}

void
callback(const char *cmd)
{
  if (strcmp(cmd, "exit")==0)
    exit(0);

  /* Open the command for reading. */
  // rl_message("running %s %d\n", cmd, strlen(cmd));
  //rl_message("\n");
}
