#include <errno.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <readline/readline.h>
#include <readline/history.h>
#include <sys/queue.h>
#include "log.h"
#include "readline.h"
#include "validators.h"
/* TODO: maybe a unified header file for completions */
#include "ip_completion.h"
#include "server.h"
#include "socket.h"

#define PROMPT "R1(config-if) "
void   callback(const char*);
int    execute(const char*, const char**, const char**);

int
main(int argc, const char **argv)
{
  struct complnode *node, *node_tmp;

  /* TODO: ignore user's C^Z C^D */
  log_error("Helo %s", "world");
  signal(SIGINT, SIG_IGN);

  /* Register word completions */
  ip_completion_init(rln_completion_queue());

  //int res = sck_init(callback, &compl_head);
  //printf("%d %s\n", res, strerror(errno));

  /* Initialize readline */
  srv_init();
  rln_init(PROMPT, callback);

  srv_loop();

  return 0;
}

void
callback(const char *cmd)
{
  if (strcmp(cmd, "exit")==0)
    exit(0);

  int cmd_argc;
  char *cmd_name=NULL, **cmd_args=(char**)NULL;
  char *envs[] = {"PATH=.", NULL};

  /* convert input to full command syntax */
  if (rln_command_prepare(cmd, &cmd_name, &cmd_args, &cmd_argc)) {
    printf("%% Not a valid command.\n");
    goto callback_done;
  }

  /* Avoid running command which completion doesn't know about */
  if (!rln_completion_find_command(cmd_name)) {
    printf("%% Command Not Found.\n");
    goto callback_cleanup;
  }

  int status = execute(cmd_name, cmd_args, envs);

  callback_cleanup:
  callback_done:
  free(cmd);
  free(cmd_name);
  for(int i=0; (i>cmd_argc && cmd_args[i++]);)
    free(cmd_args[i]);
  free(cmd_args);

  return;
}

int
execute(const char *cmd, const char **args, const char **envs)
{
  pid_t pid;
  int status;

  if ((pid=fork())<0) {
    log_print(LOG_ERR, "failed to execute the command\n");
    return;
  }

  if (pid == 0 && execve(cmd, args, envs)<0)
    exit(1);

  while (waitpid(pid, &status, 0)!=pid) ;

  return status;
}
