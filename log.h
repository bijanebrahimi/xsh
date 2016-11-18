#ifndef _LOG_H_
#define _LOG_H_

#define LogALL   0xff
#define LOG_INFO  0x01
#define LOG_DEBUG 0x02
#define LOG_WARN 0x04
#define LOG_ERR 0x08

#define log_error(format, ...)    log_print(LOG_ERR,   format, __VA_ARGS__)
#define log_info(format, ...)     log_print(LOG_INFO,  format, __VA_ARGS__)
#define log_debug(format, ...)    log_print(LOG_DEBUG, format, __VA_ARGS__)
#define log_warning(format, ...)  log_print(LOG_WARN,  format, __VA_ARGS__)


extern void log_print(int, const char*, ...);

#endif
