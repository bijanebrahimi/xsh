#include <stdio.h>
#include "readline.h"
#include "validators.h"
#include "ip_completion.h"

int
ip_completion_init(struct complhead *compl_head)
{
  struct complnode *node_no;


  /* FIXME: TODO: complete the new completion_add2 and remove the old one */
  rln_completion_add("ip address <ip> <netmask> [secondary]",
                      "IP Manipulating Command",
                      "IP Address",
                      "IPv4 address",
                      "IPv4 netmask address",
                      "IPv4 alias");
  rln_completion_add("ip address <ip> <network> [secondary]",
                      NULL,
                      NULL,
                      NULL,
                      "IPv4 network number",
                      "IPv4 alias");
  return 0;
}
