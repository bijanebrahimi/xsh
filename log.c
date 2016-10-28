#include <math.h>
#include <stdio.h>
#include <stdarg.h>
#include <sys/types.h>
#include "log.h"

struct testss{
  u_long level;
  const char *name;
} log_names[] = {
  {LogINFO, "INFO"},
  {LogDEBUG, "DEBUG"},
  {LogERROR, "ERROR"},
  {0, '\0'}
};

#ifdef DEBUG
int log_levels = (int)(LogALL);
#else
int log_levels = (int)(LogINFO|LogERROR);
#endif

extern void
log_print(int level, const char *fmt, ...)
{
  if (!(log_levels&level))
    return ;
  int log_index, log_descriptor;
  const char *log_name;
  log_index = (int)log2((double)level);
  log_name = log_names[log_index].name;
  va_list ap;
  va_start(ap, fmt);
  if (level&LogERROR)
    vfprintf(stderr, fmt, ap);
  else
    vfprintf(stdout, fmt, ap);
  va_end(ap);
}

extern void
log_dump(int level, const char *buff, size_t buff_len)
{
  int i;
  for (i=0; i<buff_len; i++)
    fprintf(stdout, "%c", buff[i]);
  fprintf(stdout, "\n");
}
