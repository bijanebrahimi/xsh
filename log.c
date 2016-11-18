#include <math.h>
#include <stdio.h>
#include <stdarg.h>
#include <sys/types.h>
#include "log.h"

struct{
  u_long level;
  const char *label;
} log_labels[] = {
  {LOG_INFO,   "INFO: "},
  {LOG_DEBUG,  "DEBUG: "},
  {LOG_DEBUG,  "WARNING: "},
  {LOG_ERR,    "ERROR: "}
};

int log_levels = (int)(LOG_ALL);

extern void
log_print(int level, const char *fmt, ...)
{
  if (!(log_levels&level))
    return ;

  int log_index, log_descriptor;
  const char *log_label, *fmt_tmp=NULL;

  /* Get log label */
  log_index = (int)log2((double)level);
  log_label = log_labels[log_index].label;

  /* get variable list */
  va_list ap;
  va_start(ap, fmt);

  /* prepare new temporary format string */
  fmt_tmp = malloc(strlen(fmt)+strlen(log_label)+1);
  if (!fmt_tmp)
    return;

  /* inject log label info format string */
  sprintf(fmt_tmp, "%s%s\n", log_label, fmt);
  if (level&LOG_ERR)
    vfprintf(stderr, fmt_tmp, ap);
  else
    vfprintf(stdout, fmt_tmp, ap);

  /* cleanup */
  free(fmt_tmp);
  va_end(ap);
}
