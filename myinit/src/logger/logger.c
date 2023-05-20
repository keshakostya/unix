#include <stdlib.h>
#include <stdio.h>
#include <stdarg.h>
#include "logger.h"


void log_msg(FILE *log_fp, const char *format, ...)
{
        va_list argp;

        va_start(argp, format);
        vfprintf(log_fp, format, argp);
        va_end(argp);
        fflush(log_fp);
}
