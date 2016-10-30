#ifndef _READLINE_H_
#define _READLINE_H_

extern int readline_init(const char*, void (*)(const char*),
                         char (**)(const char*, int ,int));

#endif // _READLINE_H_
