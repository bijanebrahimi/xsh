#ifndef _LOG_H_
#define _LOG_H_

#define LOG_ALL   0xff
#define LOG_INFO  0x01
#define LOG_DEBUG 0x02
#define LOG_WARN  0x04
#define LOG_ERR   0x08

#define log_error(...)    log_print(LOG_ERR,   __VA_ARGS__)
#define log_info(...)     log_print(LOG_INFO,  __VA_ARGS__)
#define log_debug(...)    log_print(LOG_DEBUG, __VA_ARGS__)
#define log_warning(...)  log_print(LOG_WARN,  __VA_ARGS__)


extern void log_print(int, const char*, ...);

#endif
