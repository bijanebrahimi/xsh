#ifndef _LOG_H_
#define _LOG_H_

#define LogALL   0xff
#define LogINFO  0x01
#define LogDEBUG 0x02
#define LogERROR 0x04

extern void log_print(int, const char*, ...);
extern void log_dump(int, const char*, size_t);

#endif
