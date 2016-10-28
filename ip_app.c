#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <readline/readline.h>
#include <readline/history.h>
#include "log.h"
#include "readline.h"

#define PROMPT "\e[0m\e[93m(config-if-ip)\e[0m "

void   callback(const char *cmd);
char **completion(const char*, int ,int);
char  *my_generator(const char* text, int state);
char  *dupstr(char* s);
void  *xmalloc(int size);

char  *cmd[] = {"ifconfig", "shutdown", "no shutdown" ,"word", "exit"};


int
main(int argc, const char **argv)
{
  /* Ignore user's interruption signal */
  signal(SIGINT, SIG_IGN);

  readline_init(PROMPT, callback, completion);
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
  add_history(cmd);
  sprintf(command, "/usr/bin/ifconfig\n");
  fp = popen(command, "r");
  if (fp == NULL) {
    printf("Failed to run command\n" );
    exit(1);
  }

  /* Read the output a line at a time - output it. */
  while (fgets(output, sizeof(output)-1, fp) != NULL) {
    printf("%s", output);
  }

  /* close */
  pclose(fp);
  printf("\n");
  exit(0);
}

char**
completion(const char *text, int start, int end)
{
  char **matches;

  matches = (char **)NULL;
  //printf("[%s %d:%d]\n", text, start, end);
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
  //printf("[%s %d]\n", text, state);
  //return (char*)NULL;

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

  r = (char*)xmalloc((strlen(s) + 1));
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
