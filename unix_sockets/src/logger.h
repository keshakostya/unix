#ifndef __LOGGER_H
#define __LOGGER_H
#include <stdio.h>

#define log_write(log_file, ...) write_log_message((log_file), __VA_ARGS__)

void write_log_message(FILE *log_file, const char *format, ...);

FILE *create_log_file(const char *log_filename);

#endif /* __LOGGER_H */