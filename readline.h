#ifndef _READLINE_H_
#define _READLINE_H_

typedef void (*readline_callback_t)(const char*);
typedef char (**my_completion)(const char*, int ,int);
extern int readline_init(const char *prompt, void (*callback)(const char*),
                         char **my_completion(const char*, int ,int));

#endif // _READLINE_H_
