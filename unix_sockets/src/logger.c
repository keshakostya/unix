#include <stdio.h>
#include <stdarg.h>
#include "logger.h"

FILE *create_log_file(const char *log_filename)
{
        FILE *log_file = NULL;

        if (log_filename == NULL) {
                return NULL;
        }

        log_file = fopen(log_filename, "w");
        if (log_file == NULL) {
                return NULL;
        }

        return log_file;
}

void write_log_message(FILE *log_file, const char *format, ...)
{
        va_list argp;

        va_start(argp, format);
        vfprintf(log_file, format, argp);
        va_end(argp);

        fflush(log_file);
}