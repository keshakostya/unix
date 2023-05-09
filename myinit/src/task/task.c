#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <signal.h>
#include <sys/types.h>
#include "task.h"

#define split_and_copy(line, delim) strdup(strtok(line, delim))

#define is_path_absolute(path) (path[0] == '/')

size_t count_tokens(const char *line)
{
        char *line_dup = NULL;
        size_t count = 0;

        line_dup = strdup(line);
        if (!line_dup) {
                return count;
        }

        if (!strtok(line_dup, " ")) {
                goto out;
        }

        count = 1;
        while(strtok(NULL, " ")) {
                count++;
        }
out:
        free(line_dup);
        return count;
}

struct Task *task_create(char *config_line)
{
        struct Task *task;

        task = malloc(sizeof(struct Task));
        if (!task) {
                return NULL;
        }

        task->argc = count_tokens(config_line) - 2;
        if (task->argc < 1) {
                goto err_free_task;
        }

        task->argv = malloc(sizeof(char *) * (task->argc + 1));
        if (!task->argv) {
                goto err_free_task;
        }

        task->argv[0] = split_and_copy(config_line, " ");
        if (!task->argv[0] || !is_path_absolute((task->argv[0]))) {
                goto err_free_task;
        }

        for (int i = 1; i < task->argc; ++i) {
                task->argv[i] = split_and_copy(NULL, " ");
                if (!task->argv[i]) {
                        goto err_free_task;
                }
        }
        task->argv[task->argc] = NULL;

        task->input = split_and_copy(NULL, " ");
        if (!task->input || !is_path_absolute((task->input))) {
                goto err_free_task;
        }

        task->output = split_and_copy(NULL, " ");
        if (!task->output || !is_path_absolute((task->output))) {
                goto err_free_task;
        }

        task->pid = 0;
        return task;
err_free_task:
        task_destroy(task);
        return NULL;
}

int task_start(struct Task *task)
{
        pid_t pid = 0;

        pid = fork();
        switch (pid) {
        case 0:
                if (!freopen(task->input, "r", stdin)) {
                        exit(EXIT_FAILURE);
                }

                if (!freopen(task->output, "a+", stdout)) {
                        exit(EXIT_FAILURE);
                }

                /* execv never returns on success */
                execv(task->argv[0], task->argv);
                exit(EXIT_FAILURE);
        case -1:
                return -1;
        default:
                task->pid = pid;
                return 0;
        }
}

int task_stop(struct Task *task)
{
        if (!task->pid) {
                return -1;
        }

        return kill(task->pid, SIGKILL);
}

void task_destroy(struct Task *task)
{
        if (!task) {
                return;
        }

        task_stop(task);

        if (task->argv) {
                for (int i = 0; i < task->argc; ++i) {
                        if (task->argv[i]) {
                                free(task->argv[i]);
                        }
                }

                free(task->argv);
        }

        if (task->input) {
                free(task->input);
        }

        if (task->output) {
                free(task->output);
        }

        free(task);
}