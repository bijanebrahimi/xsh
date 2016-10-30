#ifndef _COMPLETION_H_
#define _COMPLETION_H_

#define MAXCOMPNAMESIZE 32
#define MAXCOMPDESCSIZE 128

#include <sys/queue.h>

TAILQ_HEAD(comphead, compnode);

struct compnode {
  char name[MAXCOMPNAMESIZE];
  char description[MAXCOMPDESCSIZE];

  TAILQ_ENTRY(compnode) next;
  struct comphead childs;
};


extern int                rln_init(const char*,
                                   void (*)(const char*),
                                   char (*)(const char*, int ,int));
extern struct compnode   *rln_completion_insert(const char*, const char*, struct comphead*);
extern struct compnode   *rln_completion_find_name(const char*, struct comphead*);

#endif // _COMPLETION_H_
