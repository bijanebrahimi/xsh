#include <stdio.h>
#include "readline.h"
#include "validators.h"
#include "ip_completion.h"

int
ip_completion_init(struct complhead *compl_head)
{
  struct complnode *node_no;

  const struct complnode _no[] = {
    {"no", COMPLTYPE_STATIC, 0, NULL, NULL, "no", "Undo command"},
    {NULL}
  };
  node_no = rln_completion_add(_no, compl_head);

  const struct complnode _ip_address_netmask[] = {
    {"ip", COMPLTYPE_STATIC, 0, NULL, NULL, "ip", "IP Manipulating Command"},
    {"address", COMPLTYPE_STATIC, 0, NULL, NULL, "address", "IP Address Manipulting Command"},
    {"<ip>", COMPLTYPE_VARIABLE, 0, NULL, validator_ip, "A.B.C.D", "IPv4 Address"},
    {"<netmask>", COMPLTYPE_VARIABLE, 0, NULL, validator_ip, "A.B.C.D", "IPv4 Netmask Address"},
    {"secondary", COMPLTYPE_STATIC, 1, NULL, NULL, "secondary", "IPv4 Alias"},
    {NULL}
  };
  rln_completion_add(_ip_address_netmask, compl_head);
  rln_completion_add(_ip_address_netmask, &node_no->head);

  const struct complnode _ip_address_network[] = {
    {"ip"},
    {"address"},
    {"<ip>", COMPLTYPE_VARIABLE, 0, NULL, NULL, "A.B.C.D", "IPv4 Address"},
    {"<network>", COMPLTYPE_VARIABLE, 0, NULL, validator_network, "1-32", "Network Number"},
    {"secondary", COMPLTYPE_STATIC, 1, NULL, NULL, "secondary", "IPv4 Alias"},
    {NULL}
  };
  rln_completion_add(_ip_address_network, compl_head);
  rln_completion_add(_ip_address_network, &node_no->head);

  const struct complnode _ip_defaultgw[] = {
    {"ip"},
    {"default-gateway", COMPLTYPE_STATIC, 0, NULL, NULL, "default-gateway", "Default IPv4 Gateway"},
    {"<ip>", COMPLTYPE_VARIABLE, 0, NULL, NULL, "A.B.C.D", "IPv4 Address"},
    {NULL}
  };
  rln_completion_add(_ip_defaultgw, compl_head);
  rln_completion_add(_ip_defaultgw, &node_no->head);
}
