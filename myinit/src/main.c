#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>
#include <signal.h>
#include <wait.h>
#include "configuration/configuration.h"
#include "task/task.h"
#include "logger/logger.h"
#include "error/error.h"

static char *config_path            = NULL;
static struct Configuration *config = NULL;
static size_t tasks_count           = 0;
static struct Task **tasks          = NULL;
static FILE *log_fp                 = NULL;

void start();

int setup_log_file()
{
        log_fp = fopen("/tmp/myinit.log", "a+");
        if (!log_fp) {
                return LOG_FILE_OPEN_ERR;
        }

        return OK;
}

int setup_configuration()
{
        config = configuration_read(config_path);
        if (!config || !config->len) {
                return CONFIG_READ_ERR;
        }
        
        return OK;
}

int setup_tasks()
{
        struct ConfigurationNode *n = NULL;

        tasks = malloc(sizeof(struct Task *) * config->len);
        if (!tasks) {
                return TASKS_CREATE_ERR;
        }

        tasks_count = config->len;
        n = config->head;
        for (size_t i = 0; i < tasks_count && n; ++i) {
                tasks[i] = task_create(n->line);
                if (!tasks[i]) {
                        return TASKS_CREATE_ERR;
                }
                n = n->next;
        }

        return OK;
}

int setup_myinit()
{
        int err = OK;

        err = setup_log_file();
        if (err) {
                return err;
        }

        err = setup_configuration(config_path);
        if (err) {
                return err;
        }

        err = setup_tasks();
        if (err) {
                return err;
        }

        return OK;
}

void clean_up()
{
        configuration_destroy(config);
        config = NULL;

        for (size_t i = 0; i < tasks_count; ++i) {
                task_destroy(tasks[i]);
        }
        free(tasks);
        tasks = NULL;

        fclose(log_fp);
        log_fp = NULL;
}

void err_and_die(int err)
{
        LOG(log_fp, "%s\n", error_stringify(err));

        clean_up();

        exit(EXIT_FAILURE);
}

void restart(int signum)
{
        LOG(log_fp, "Caught SIGHUP (%d). Restarting...\n", signum);

        clean_up();

        start();
}

void start()
{
        int err = OK;
        int wstatus;
        pid_t pid = 0;

        err = setup_myinit();
        if (err) {
                err_and_die(err);
        }

        if (chdir("/")) {
                LOG(log_fp, "Failed to chdir to /\n");
                exit(EXIT_FAILURE);
        }

        for (size_t i = 0; i < tasks_count; ++i) {
                err = task_start(tasks[i]);
                if (err) {
                        LOG(log_fp, "Failed to start child [%s]\n", tasks[i]->argv[0]);
                        err_and_die(err);
                }

                LOG(log_fp, "Started child [%s] with pid %d\n", 
                    tasks[i]->argv[0], tasks[i]->pid);
        }

        signal(SIGHUP, restart);

        while (1)
        {
                pid = waitpid(-1, &wstatus, 0);
                for (size_t i = 0; i < tasks_count; ++i) {
                        if (pid == tasks[i]->pid) {
                                LOG(log_fp, 
                                    "Child [%s]"
                                    " with pid %d exited with status %d."
                                    " Restarting task\n",
                                    tasks[i]->argv[0], pid, wstatus);
                                task_start(tasks[i]);
                                LOG(log_fp, "Started child [%s] with pid %d\n", 
                                        tasks[i]->argv[0], tasks[i]->pid);
                                break;
                        }
                } 
        }
        
}

int main(int argc, char **argv)
{
        char *progname = NULL;
        pid_t pid = 0;

        progname = argv[0];
        if (argc != 2) {
                printf("Usage: %s CONFIG\n", progname);
                return EXIT_FAILURE;
        }

        config_path = argv[1];
        pid = fork();
        switch (pid) {
        case 0:
                start();
                return EXIT_SUCCESS;
        case -1:
                printf("Failed to start %s\n", progname);
                return EXIT_FAILURE;
        default:
                return EXIT_SUCCESS;
        }
}