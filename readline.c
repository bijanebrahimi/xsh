#include <stdio.h>
#include <stdlib.h>
#include <readline/readline.h>
#include <readline/history.h>
#include "readline.h"


int
readline_init(const char *prompt, void (*callback)(const char*),
              char **completion(const char*, int ,int))
{
  char *buf;

  /* gnu readline initialization */
  //rl_bind_key('\t',rl_abort);
  rl_attempted_completion_function = completion;

  while ((buf = readline(prompt))!=NULL) {
    rl_bind_key('\t', rl_complete);
    callback(buf);
  }
  free(buf);

  return 0;
}
