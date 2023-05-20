#ifndef __TASK_H
#define __TASK_H

#include <sys/types.h>

struct Task {
        /* Arguments count */
        int argc;
        /* Arguments. First - progname, Last - null */
        char **argv;
        /* Input filename */
        char *input;
        /* Output filename */
        char *output;
        /* Process pid assosiated with task */
        pid_t pid;
};

struct Task *task_create(char *config_line);

int task_start(struct Task *task);

int task_stop(struct Task *task);

void task_destroy(struct Task *task);

#endif /* __TASK_H */