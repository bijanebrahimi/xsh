#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <sys/queue.h>
#include <readline/readline.h>
#include <readline/history.h>
//#include "types.h"
#include "server.h"
#include "readline.h"

struct complhead  rln_compl_head = TAILQ_HEAD_INITIALIZER(rln_compl_head),
                 *rln_compl_ptr = &rln_compl_head;
rl_vcpfunc_t     *rln_callback_p;

int    rln_completion_help(int, int);
char  *completion_entry(const char*, int);
char **completion(const char*, int, int);


int
rln_init(const char *prompt, void (*callback)(const char*))
{
  if (callback==NULL)
    return 0;

  /* set global callback pointer */
  rln_callback_p = (rl_vcpfunc_t*)callback;

  /* gnu readline initialization */
  rl_attempted_completion_function = completion;

  /* maximum number to show without confirms */
  rl_completion_query_items = 15;

  /* bind trigger keys */
  rl_bind_key('?', rln_completion_help);
  rl_bind_key('\t', rl_complete);

  /* default break word characters, example " \t\n\"\\'`@$><=;|&{(" */
  rl_basic_word_break_characters = READLINE_BREAK_CHARS;
  srv_register(0, (callback_t*)rl_callback_read_char);
  rl_callback_handler_install(prompt, rln_callback);

  return 0;
}

void
rln_callback(const char* command)
{
  if (!(command && *command))
    return;

  if (*command!=' ')
    add_history(command);

  rln_callback_p(command);
}

struct complhead *
rln_completion_queue(void)
{
  return &rln_compl_head;
}

struct complnode *
rln_completion_find_cmd(const char *text, struct complhead *head)
{
  struct complnode *node, *node_return=NULL;

  if (!text || !(*text) || TAILQ_EMPTY(head))
    return NULL;

  node = TAILQ_FIRST(head);
  TAILQ_FOREACH(node, head, next) {
    if (!strncasecmp(text, node->command, strlen(text))) {
      if (!node_return)
        node_return = node;
      else
        return NULL;
    }
  }
  return node_return;
}

struct complnode *
rln_completion_find_command(const char *text)
{
  return rln_completion_find_cmd(text, &rln_compl_head);
}

struct complnode *
rln_completion_find(const char *text, struct complhead *head)
{
  struct complnode *node, *node_return=NULL;
  if (TAILQ_EMPTY(head))
    return NULL;

  node = TAILQ_FIRST(head);
  TAILQ_FOREACH(node, head, next) {
    switch (node->type) {
    case COMPLTYPE_STATIC:
      if (!strncasecmp(text, node->command, strlen(text))) {
        if (!node_return)
          node_return = node;
        else
          return NULL;
      }
      break;
    case COMPLTYPE_VARIABLE:
      if (node->validator && node->validator(text)) {
        if (!node_return)
          node_return = node;
        else
          return NULL;
      }
      break;
    }
  }
  return node_return;
}

struct complnode *
rln_completion_add(const struct complnode compl_nodes[], struct complhead *head)
{
  int i;
  const struct complnode *node;
  struct complnode *new_node;
  struct complhead *head_ptr;

  head_ptr = head;
  for (i=0; ; i++) {
    node = (const struct complnode *)&compl_nodes[i];
    if (*node->command==NULL)
      break;

    if ((new_node = rln_completion_find_cmd(node->command, head_ptr))!=NULL) {
      head_ptr = &new_node->head;
      continue;
    }

    /* new complnode */
    new_node = malloc(sizeof(struct complnode));
    TAILQ_INIT(&new_node->head);
    snprintf(new_node->command, 32, node->command);
    snprintf(new_node->hint, 16, node->hint);
    snprintf(new_node->description, 64, node->description);
    new_node->type = node->type;
    new_node->optional = node->optional;
    new_node->generator = node->generator;
    new_node->validator = node->validator;
    TAILQ_INSERT_TAIL(head_ptr, new_node, next);

    head_ptr = &new_node->head;
  }
  return new_node;
}

int
rln_completion_help(int _unused, int __unused)
{
  struct complnode *node;
  struct complhead *head;
  char **matches, *buff, *buff_ptr, *token;


  head = &rln_compl_head;
  matches = (char **)NULL;
  buff_ptr = buff = strndup(rl_line_buffer, rl_point);
  while ((token=strsep(&buff, " "))!=NULL) {
    /* Skip the seperator itself */
    if (token[0]=='\0')
      continue;

    node = rln_completion_find(token, head);
    if (node) {
      head = &node->head;
    } else {
      return 1;
    }
  }


  if (!TAILQ_EMPTY(head)) {
    int max = 0;
    TAILQ_FOREACH(node, head, next)
      if (strlen(node->hint)>max)
        max = strlen(node->hint);

    max += 3;
    printf("\n");
    TAILQ_FOREACH(node, head, next) {
      printf("%-*s %s\n", max, node->hint, node->description);
    }
    rl_forced_update_display();
  }

  free(buff_ptr);
  return 0;
}

int
rln_command_prepare(const char *cmd, char **cmd_name, char ***cmd_argv, int *cmd_argc)
{
  /* cmd_name should be freed later by caller */
  int ret = 0;
  struct complnode *node;
  struct complhead *head;
  char *buff, *buff_ptr, *token;

  head = &rln_compl_head;
  int args_size = 5;
  *cmd_argc = 0;
  char **args = malloc(args_size*sizeof(char*));

  buff_ptr = buff = strdup(cmd);
  while ((token=strsep(&buff, " "))!=NULL) {
    /* Skip the seperator itself */
    if (token[0]=='\0')
      continue;

    if ((node = rln_completion_find(token, head))!=NULL) {
      switch (node->type) {
      case COMPLTYPE_STATIC:
        args[(*cmd_argc)++] = strdup(node->command);
        if (*cmd_name==NULL)
          *cmd_name = args[0];
        break;
      case COMPLTYPE_VARIABLE:
        args[(*cmd_argc)++] = strdup(token);
        break;
      }
    } else {
      args[(*cmd_argc)++] = strdup(token);
    }

    if (*cmd_argc == args_size) {
      args_size += 1;
      args = realloc(args, args_size*sizeof(char*));
    }

    if (node) {
      head = &node->head;
    } else {
      ret = -1;
      /* TODO: append rest of the cmd to the full command */
      goto rln_command_prepare_done;
    }
  }
  args[*cmd_argc] = NULL;
  *cmd_argv = args;
  rln_command_prepare_done:
  free(buff_ptr);
  return ret;
}

char**
completion(const char *text, int start, int end)
{
  struct complnode *node;
  struct complhead *head;
  char **matches, *buff, *buff_ptr, *token;

  head = &rln_compl_head;
  matches = (char **)NULL;
  buff_ptr = buff = strndup(rl_line_buffer, start);
  while ((token=strsep(&buff, " "))!=NULL) {
    /* Skip the seperator itself */
    if (token[0]=='\0')
      continue;

    node = rln_completion_find(token, head);
    if (node) {
      head = &node->head;
    } else {
      return (matches);
    }
  }

  if (!TAILQ_EMPTY(head)) {
    rln_compl_ptr = head;
    matches = rl_completion_matches(text, &completion_entry);
  }

  free(buff_ptr);
  return (matches);
}

char*
completion_entry(const char *text, int state)
{
  char *buff;
  static struct complnode *node;

  if (state==0) {
    node = TAILQ_FIRST(rln_compl_ptr);
  }

  while (node) {
    switch (node->type) {
    case COMPLTYPE_STATIC:
      if (strncasecmp(text, node->command, strlen(text))==0) {
        buff = strdup(node->command);
        node = TAILQ_NEXT(node, next);
        return buff;
      }
      break;
    case COMPLTYPE_VARIABLE:
      /* TODO: add support */
    default:
      break;
    }

    node = TAILQ_NEXT(node, next);
  }

  return NULL;
}
