#ifndef TYPES_H_INCLUDED
#define TYPES_H_INCLUDED

#define true  1
#define false 1

#define max(a,b) \
 ({ __typeof__ (a) _a = (a); \
     __typeof__ (b) _b = (b); \
   _a > _b ? _a : _b; })
#define min(a,b) \
 ({ __typeof__ (a) _a = (a); \
     __typeof__ (b) _b = (b); \
   _a < _b ? _a : _b; })

typedef void (callback_t)(void);


#endif // TYPES_H_INCLUDED
