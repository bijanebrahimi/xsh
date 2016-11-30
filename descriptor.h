#ifndef _THREAD_H_
#define _THREAD_H_

#include "types.h"

TAILQ_HEAD(descriptorq, descriptor);

typedef struct descriptor {
  int dsc_no;
  callback_t *dsc_func;

  TAILQ_ENTRY(descriptor) siblings;
} descriptor_t;

extern int dsc_init();
extern int dsc_loop();
extern int dsc_register(int, callback_t*);

#endif
