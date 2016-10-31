#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <sys/queue.h>
#include <readline/readline.h>
#include <readline/history.h>
#include "readline.h"

int
rln_help_inline(int a, int b)
{
  printf("\n[%s][%d %d]\n", rl_line_buffer, rl_point, rl_end);
  rl_forced_update_display();

  //rl_done = 1;
  //rl_on_new_line();

  //rl_message("what?\n");
}

int
rln_init(const char *prompt, void (*callback)(const char*),
              char (*completion)(const char*,int ,int))
{
  char *buf;

  /* gnu readline initialization */
  rl_attempted_completion_function = completion;
  //rl_completion_entry_function = NULL;

  /* maximum number to show without confirms */
  rl_completion_query_items = 80;

  /* default break word characters */
  //rl_basic_word_break_characters = " \t\n\"\\'`@$><=;|&{(";
  rl_basic_word_break_characters = READLINE_BREAK_CHARS;

  rl_bind_key ('\t', rl_insert);
  rl_bind_key('?', rln_help_inline);
  rl_bind_key('\t', rl_complete);
  while ((buf = readline(prompt))!=NULL) {

    /* Ignoring empty commands */
    if (buf[0]==0)
      continue;

    /* Ignore adding commands to history if spaced */
    if (buf[0]!=' ')
      add_history(buf);

    callback(buf);
  }
  free(buf);

  return 0;
}

struct compnode *
rln_completion_find_name(const char *name, struct comphead *head)
{
  struct compnode *node, *found_node=NULL;
  if (TAILQ_EMPTY(head))
    return NULL;

  node = TAILQ_FIRST(head);
  TAILQ_FOREACH(node, head, next) {
    if (strncasecmp(name, node->text, strlen(name))==0) {
      if (!found_node)
        found_node = node;
      else
        return NULL;
    }
  }
  return found_node;
}

struct compnode *
rln_completion_find_syntax(const char *syntax, struct comphead *head)
{
  struct compnode *node, *found_node=NULL;
  if (TAILQ_EMPTY(head))
    return NULL;

  node = TAILQ_FIRST(head);
  TAILQ_FOREACH(node, head, next) {
    if (strncasecmp(syntax, node->syntax, strlen(syntax))==0) {
      if (!found_node)
        found_node = node;
      else
        return NULL;
    }
  }
  return found_node;
}

int
rln_completion(const char *cmd, struct comphead *head)
{
  struct compnode *node;
  struct comphead *head_ptr;
  char *comp_text, *token, *comp_text_ptr;

  head_ptr = head;
  comp_text_ptr = comp_text = strdup(cmd);

  while ((token=strsep(&comp_text, READLINE_BREAK_CHARS))!=NULL) {
    /* Node already exists, move on */
    node = rln_completion_find_name(token, head);
    if (node) {
      head = &node->head;
      continue;
    }

    /* Skip the seperator itself */
    if (strlen(token)==1)
      continue;

    node = malloc(sizeof(struct compnode));
    sprintf(node->syntax, token);


    switch (token[0]) {
    case '<':
      /* Variable argument */
      sprintf(node->text, token);
      break;
    case '[':
      /* Optional argument */
      snprintf(node->text, strlen(token)-1, token+1);
      break;
    default:
      /* Static argument */
      sprintf(node->text, token);
      break;
    }

    TAILQ_INIT(&node->head);
    TAILQ_INSERT_TAIL(head, node, next);
    head = &node->head;
  }

  free(comp_text_ptr);
  return 0;
}
