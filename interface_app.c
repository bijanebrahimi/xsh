#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <sys/queue.h>
#include <readline/readline.h>
#include <readline/history.h>
#include "log.h"
#include "readline.h"
#include "validators.h"

#define PROMPT "R1(config-if) "
void   callback(const char*);
struct complhead compl_head = TAILQ_HEAD_INITIALIZER(compl_head);

int
main(int argc, const char **argv)
{
  struct complnode *node, *node_tmp;

  /* TODO: ignore user's C^Z C^D */
  signal(SIGINT, SIG_IGN);

  /* Register word completions */
  const struct complnode compl_ip[] = {
    {"ip", COMPLTYPE_STATIC, 0, COMPLFMT_NONE, NULL, NULL, "ip", "IP Manipulating Command"},
    {"address", COMPLTYPE_STATIC, 0, COMPLFMT_NONE, NULL, NULL, "address", "IP Address Manipulting Command"},
    {"<ip>", COMPLTYPE_VARIABLE, 0, COMPLFMT_NONE, NULL, validator_ip, "A.B.C.D", "IPv4 Address"},
    {"<netmask>", COMPLTYPE_VARIABLE, 0, COMPLFMT_NONE, NULL, validator_ip, "A.B.C.D", "IPv4 Netmask Address"},
    {"secondary", COMPLTYPE_STATIC, 1, COMPLFMT_NONE, NULL, NULL, "seondary", "IPv4 Alias"},
    {NULL}
  };
  rln_completion_add(compl_ip, &compl_head);

  const struct complnode compl_ip_network[] = {
    {"ip", COMPLTYPE_STATIC, 0, COMPLFMT_NONE, NULL, NULL, "ip", "IP Manipulating Command"},
    {"address", COMPLTYPE_STATIC, 0, COMPLFMT_NONE, NULL, NULL, "address", "IP Address Manipulting Command"},
    {"<ip>", COMPLTYPE_VARIABLE, 0, COMPLFMT_NONE, NULL, NULL, "A.B.C.D", "IPv4 Address"},
    {"<network>", COMPLTYPE_VARIABLE, 0, COMPLFMT_NONE, NULL, validator_network, "1-32", "Network Number"},
    {NULL}
  };
  rln_completion_add(&compl_ip_network, &compl_head);

  const struct complnode compl_ip_defaultgw[] = {
    {"ip", COMPLTYPE_STATIC, 0, COMPLFMT_NONE, NULL, NULL, "ip", "IP Manipulating Command"},
    {"default-gateway", COMPLTYPE_STATIC, 0, COMPLFMT_NONE, NULL, NULL, "default-gateway", "Default IPv4 Gateway"},
    {"<ip>", COMPLTYPE_VARIABLE, 0, COMPLFMT_NONE, NULL, NULL, "A.B.C.D", "IPv4 Address"},
    {NULL}
  };
  rln_completion_add(&compl_ip_defaultgw, &compl_head);

  /* Initialize readline */
  rln_init(PROMPT, callback, &compl_head);

  return 0;
}

void
callback(const char *cmd)
{
  if (strcmp(cmd, "exit")==0)
    exit(0);
}
