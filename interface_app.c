#include <errno.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include "log.h"
#include "readline.h"
/* TODO: maybe a unified header file for completions */
#include "ip_completion.h"
#include "descriptor.h"
#include "socket.h"

#define PROMPT "R1(config-if) "
void   callback(const char*);
callback_buffer_t socket_callback(caddr_t buf);
int    execute(const char*, const char**, const char**);
void   handle_termination(int);

int
main(int argc, const char **argv)
{
  struct complnode *node, *node_tmp;

  /* TODO: ignore user's C^Z C^D */
  signal(SIGINT, SIG_IGN);
  signal(SIGTERM, handle_termination);
  signal(SIGSEGV, handle_termination);
  signal(SIGABRT, handle_termination);
  signal(SIGQUIT, handle_termination);

  /*
   * Initializing server which monitors registered sockets (stdin/unix_socket) activity.
   */
  dsc_init();
  rln_init(PROMPT, callback);
  sck_init(socket_callback);

  /* Register word completions */
  ip_completion_init(rln_completion_queue());

  /* Forever loop */
  dsc_loop();

  return 0;
}

callback_buffer_t
socket_callback(caddr_t buf)
{
  log_debug("%s", buf);
  callback(buf);
  free(buf);
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

void
handle_termination(int signo)
{
  log_debug("graceful %s", "shutdown");
  sck_cleanup();
  exit(1);
}
