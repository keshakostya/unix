#ifndef __LOGGER_H
#define __LOGGER_H

#include <stdio.h>

#define LOG(log_fp, ...)  log_msg(log_fp, __VA_ARGS__)

void log_msg(FILE *log_fp, const char *format, ...);

#endif /* __LOGGER_H */