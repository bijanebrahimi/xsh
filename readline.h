#ifndef _COMPLETION_H_
#define _COMPLETION_H_

#include <sys/queue.h>

#define MAXCOMPNAMESIZE 32
#define MAXCOMPDESCSIZE 128
#define READLINE_BREAK_CHARS " /"


TAILQ_HEAD(comphead, compnode);

struct compnode {
  /* The syntaxt: <variable>,[optional] */
  char syntax[32];
  char text[32];
  int  optional;

  char description[64];

  TAILQ_ENTRY(compnode) next;
  struct comphead head;
};


extern int                rln_init(const char*,
                                   void (*)(const char*),
                                   char (*)(const char*, int ,int),
                                   int (*)(int ,int));
extern struct compnode   *rln_completion_find_name(const char*, struct comphead*);
extern struct compnode   *rln_completion_find_syntax(const char*, struct comphead*);
extern int                rln_completion(const char*, struct comphead*);

#endif // _COMPLETION_H_
