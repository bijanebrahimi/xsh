#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdarg.h>
#include <regex.h>
#include <sys/queue.h>
#include <readline/readline.h>
#include <readline/history.h>
//#include "types.h"
#include "validators.h"
#include "descriptor.h"
#include "readline.h"

struct complhead  rln_compl_head = TAILQ_HEAD_INITIALIZER(rln_compl_head),
                 *rln_compl_ptr = &rln_compl_head;
rl_vcpfunc_t     *rln_callback_p;

int    rln_completion_help(int, int);
char  *completion_entry(const char*, int);
char **completion(const char*, int, int);
void   rln_callback_read_char(int);

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
  dsc_register(0, (callback_t*)rln_callback_read_char);
  rl_callback_handler_install(prompt, rln_callback);

  return 0;
}

void
rln_callback_read_char(int fd)
{
  rl_callback_read_char();
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
rln_completion_find(const char *text, struct complhead *head)
{
  struct complnode *node, *node_return=NULL;
  if (TAILQ_EMPTY(head))
    return NULL;

  node = TAILQ_FIRST(head);
  TAILQ_FOREACH(node, head, next) {
    if (node->flags&COMMAND_VARIABLE) {
      if (node->validator && node->validator(text)) {
        if (!node_return)
          node_return = node;
        else
          return NULL;
      }
    } else if (!strncasecmp(text, node->command, strlen(text))) {
        if (!node_return)
          node_return = node;
        else
          return NULL;
    }
  }
  return node_return;
}

struct complnode*
rln_complnode(const char *command, const char *description, char flags)
{
  struct complnode *node;

  node = malloc(sizeof(struct complnode));
  memset(node, '\0', sizeof(struct complnode));
  sprintf(node->command, command);
  sprintf(node->description, description);
  node->flags = flags;
  if (node->flags&COMMAND_VARIABLE) {
    node->validator = validator_function(command);
    sprintf(node->hint, "<%s>", command);
  } else
    sprintf(node->hint, command);

  return node;
}

int
rln_completion(const char *syntax, ...)
{
  va_list ap;
  char *syntax_ptr, command[64], *description;
  size_t command_sz, head_stack_sz, head_stack_empty_sz;
  regex_t regex;
  regmatch_t regex_match[1];
  struct complhead *head, *head_stack[20];
  struct complnode *node;
  int head_stack_empty, command_optional_counter, command_variable_counter, flags;


  if (regcomp(&regex, "(\\[|\\]|>|<|[a-z]+|$)", REG_ICASE|REG_EXTENDED))
    return 1;

  va_start(ap, syntax);
  head_stack_empty = command_optional_counter = command_variable_counter = 0;
  head_stack_sz = head_stack_empty_sz = 0;

  head = &rln_compl_head;
  syntax_ptr = syntax;
  while ((syntax+strlen(syntax)>syntax_ptr) && (!regexec(&regex, syntax_ptr, 1, regex_match, REG_NOTBOL))) {
    command_sz = regex_match[0].rm_eo - regex_match[0].rm_so+1;
    snprintf(command, command_sz, syntax_ptr+regex_match[0].rm_so);
    switch (command[0]) {
    case '[':
      command_optional_counter += 1;
      head_stack[head_stack_sz++] = head;
      break;
    case ']':
      head_stack_empty = 1;
      head_stack_empty_sz += 1;
      break;
    case '<':
      command_variable_counter += 1;
      break;
    case '>':
      break;
    case '\0':
      /* End of syntax */
      break;
    default:
      if (command_variable_counter) {
        command_variable_counter -= 1;
        flags |= COMMAND_VARIABLE;
      }
      if (command_optional_counter) {
        command_optional_counter -= 1;
        flags |= COMMAND_OPTIONAL;
      }
      description = va_arg(ap, char *);
      node = rln_complnode(command, description, flags);
      TAILQ_INSERT_HEAD(head, node, next);
      head = &(node->head);
      if (head_stack_empty) {
        for (int i=0; i<head_stack_empty_sz; i++, head_stack_sz--)
          TAILQ_INSERT_TAIL(head_stack[head_stack_sz-1], node, next);
        head_stack_empty = 0;
      }
    }

    syntax_ptr += regex_match[0].rm_eo;
  }

  va_end(ap);
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
      if (node->flags|COMMAND_VARIABLE) {
        args[(*cmd_argc)++] = strdup(node->command);
        if (*cmd_name==NULL)
          *cmd_name = args[0];
      } else {
        args[(*cmd_argc)++] = strdup(token);
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
    if (node->flags|COMMAND_VARIABLE) {
      if (strncasecmp(text, node->command, strlen(text))==0) {
        buff = strdup(node->command);
        node = TAILQ_NEXT(node, next);
        return buff;
      }
    } else {
      /* TODO: add support */
    }

    node = TAILQ_NEXT(node, next);
  }

  return NULL;
}
