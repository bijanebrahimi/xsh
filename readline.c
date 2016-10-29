#include <stdio.h>
#include <stdlib.h>
#include <readline/readline.h>
#include <readline/history.h>
#include "readline.h"


int
readline_init(const char *prompt, void (*callback)(const char*),
              char **completion(const char*, int, int),
              char *completion_entry(const char*, int))
{
  char *buf;

  /* gnu readline initialization */
  //rl_attempted_completion_function = completion;
  rl_completion_entry_function = completion_entry;

  /* maximum number to show without confirms */
  rl_completion_query_items = 80;

  /* default break word characters */
  //rl_basic_word_break_characters = " \t\n\"\\'`@$><=;|&{(";

  while ((buf = readline(prompt))!=NULL) {
    rl_bind_key('\t', rl_complete);

    /* Ignore adding commands to history if spaced */
    if (buf[0]!=' ')
      add_history(buf);

    callback(buf);
  }
  free(buf);

  return 0;
}
