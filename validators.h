#ifndef _VALIDATORS_H_
#define _VALIDATORS_H_

typedef int (*validator_t)(const char*);

extern validator_t    validator_function(const char *name);
extern int            validator_ip(const char*);
extern int            validator_network(const char*);

#endif
