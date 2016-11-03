#include <stdio.h>
#include <regex.h>


int
validator_ip(const char *ip)
{
  regex_t regex;
  char msgbuf[100];

  if (regcomp(&regex, "^1.1.1.1", 0)) {
    return 0;
  }
  return !regexec(&regex, ip, 0, NULL, 0);
}

int
validator_network(const char *network)
{
  regex_t regex;
  char msgbuf[100];

  if (regcomp(&regex, "^3[0-2]", 0)) {
    return 0;
  }
  return !regexec(&regex, network, 0, NULL, 0);
}
