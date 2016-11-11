#ifndef _COMPLETION_H_
#define _COMPLETION_H_

#include <sys/queue.h>

#define MAXCOMPNAMESIZE 32
#define MAXCOMPDESCSIZE 128
#define READLINE_BREAK_CHARS " /"


typedef enum {COMPLTYPE_STATIC, COMPLTYPE_VARIABLE} compltype_t;
typedef enum {false, true} boolean_t;

TAILQ_HEAD(complhead, complnode);
typedef struct complnode {
  /* The syntaxt: <variable>,[optional] */
  char command[32];     /* apears in command */
  int  type;            /* static/variable */
  int  optional;        /* mandtory/optional */
  char* (*generator)(const char*, int); /* completion generator function */
  int (*validator)(const char*);        /* validator function */
  char hint[16];    /* short help text */
  char description[64]; /* long help text */

  TAILQ_ENTRY(complnode) next;
  struct complhead head;
} complnode_t;


extern int                 rln_init(const char*,
                                    void (*)(const char*),
                                    struct complhead*);
extern struct complnode   *rln_completion_find_cmd(const char*, struct complhead*);
extern struct complnode   *rln_completion_find_syntax(const char*, struct complhead*);
extern struct complnode   *rln_completion_add(const struct complnode[], struct complhead*);
extern int                 rln_command_prepare(const char*, char**, char***, int*);

#endif // _COMPLETION_H_
