#ifndef _COMPLETION_H_
#define _COMPLETION_H_

#include <sys/queue.h>
#include "validators.h"

#define MAXCOMPNAMESIZE 32
#define MAXCOMPDESCSIZE 128
#define READLINE_BREAK_CHARS " /"


typedef enum {COMPLTYPE_STATIC, COMPLTYPE_VARIABLE} compltype_t;

TAILQ_HEAD(complhead, complnode);
typedef struct complnode {
  /* The syntaxt: <variable>,[optional] */
  char command[32];     /* apears in command */
  int  type;            /* static/variable */
  int  optional;        /* mandtory/optional */
  char* (*generator)(const char*, int); /* completion generator function */
  validator_t validator;               /* validator function */
  char hint[16];    /* short help text */
  char description[64]; /* long help text */

  TAILQ_ENTRY(complnode) next;
  struct complhead head;
} complnode_t;


extern int                 rln_init(const char*,
                                    void (*)(const char*));
extern void                rln_callback(const char*);
extern struct complhead   *rln_completion_queue(void);
extern struct complnode   *rln_completion_find_command(const char*);
extern int                 rln_completion_add(const char*, ...);

#endif // _COMPLETION_H_
