#include <stdio.h>
#include <stdlib.h>
#include <error.h>
#include <argp.h>


/* Internal struct to keep parsed arguments */
typedef enum {IP_ADDRESS, IP_DEFAULTGATEWAY} ipcmd_t;
struct arguments {
  ipcmd_t command;
  char **parameters;
};

static error_t parse_opt(int, char*, struct argp_state*);

const char *argp_program_version = "IP manipulating command, v0.1.0";
static char doc_args[] = "command [Options...]";
static char doc_help[] =
  "IP manipulating command"
  "\v"
  "to be continue...";
static struct argp_option options[] = {
  { 0 }
};
/* argp parser */
static struct argp arg_parser = {options, parse_opt, doc_args, doc_help};


int
main(int argc, const char **argv)
{
  int i;
  struct arguments app_args;
  argp_parse(&arg_parser, argc, argv, 0, 0, &app_args);

  switch (app_args.command) {
  case IP_ADDRESS:
    printf("ip address command\n");
    break;
  case IP_DEFAULTGATEWAY:
    printf("ip default-gateway command\n");
    break;
  }

  for (i=0; app_args.parameters[i]; i++)
    printf("  param(%d): %s\n", i, app_args.parameters[i]);
  return 0;
}

/* Parse a single option. */
static error_t
parse_opt (int key, char *arg, struct argp_state *state)
{
  struct arguments *app_args = state->input;

  switch (key) {
    case ARGP_KEY_NO_ARGS:
      argp_usage(state);

    case ARGP_KEY_ARG:
      if (!strcasecmp(arg, "address"))
        app_args->command = IP_ADDRESS;
      else if (!strcasecmp(arg, "default-gateway"))
        app_args->command = IP_DEFAULTGATEWAY;

      app_args->parameters = &state->argv[state->next];
      state->next = state->argc;
      break;

    default:
      return ARGP_ERR_UNKNOWN;
    }
  return 0;
}
