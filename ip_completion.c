#include <stdio.h>
#include "readline.h"
#include "validators.h"
#include "ip_completion.h"

int
ip_completion_init(struct complhead *compl_head)
{
  rln_completion_add("ip address <ip> <netmask> [secondary]",
                     "IP manipulating command",
                     "IP 0"0,00
                     "IPv4 address",
                     "IPv4 netmask address",
                     "IPv4 alias");
  rln_completion_add("ip address <ip> <network> [secondary]",
                     NULL,
                     NULL,
                     NULL,
                     "IPv4 network number",
                     "IPv4 alias");
  rln_completion_add("no ip address",
                     "Undo command",
                     "Undo IP manipulating command",
                     "Remove current IP address");
  rln_completion_add("no ip address <ip> <netmask> [secondary]",
                     NULL,
                     NULL,
                     NULL,
                     "IPv4 address",
                     "IPv4 netmask address",
                     "IPv4 alias");
  rln_completion_add("no ip address <ip> <network> [secondary]",
                     NULL,
                     NULL,
                     NULL,
                     NULL,
                     "IPv4 network number",
                     "IPv4 alias");
  return 0;
}
