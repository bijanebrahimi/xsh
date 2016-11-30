#include <stdio.h>
#include <stdlib.h>
#include <regex.h>
#include <errno.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include "validators.h"


validator_t
validator_function(const char *name)
{
  if (!strcmp(name, "<ip>") || !strcmp(name, "<netmask>"))
    return validator_ip;
  else if (strcmp(name, "<network>")==0)
    return validator_network;
  return NULL;
}

int
validator_ip(const char *ip)
{
  unsigned char buf[sizeof(struct in_addr)];
  return inet_pton(AF_INET, ip, buf)==1? 1: 0;
}

int
validator_network(const char *network)
{
  char *end_ptr = NULL;
  int net;

  net = strtol(network, &end_ptr, 10);
  if (errno==ERANGE || (net>32 || net<1) || *end_ptr)
    return 0;
  return 1;
}

int
validator_network_old(const char *network)
{
  regex_t regex;
  char msgbuf[100];

  if (regcomp(&regex, "^(3[0-2]|[1-2]?[0-9])", REG_ICASE|REG_EXTENDED|REG_NOSUB)) {
    return 0;
  }
  return !regexec(&regex, network, 0, NULL, 0);
}
