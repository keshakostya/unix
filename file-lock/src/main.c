#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <signal.h>
#include "file_lock.h"

static struct FileLock *lock = NULL;

void sigint_handler(__attribute__((unused))  int signum)
{
        if (lock == NULL) {
                exit(EXIT_SUCCESS);
        }

        if (access(lock->lock_filename, F_OK) == 0) {
                unlink(lock->lock_filename);
        }

        destroy_lock(lock);
        exit(EXIT_SUCCESS);
}


int main(int argc, char **argv)
{
        FILE *file = NULL;
        int err = 0;
        char *filename = NULL;

        signal(SIGINT, sigint_handler);

        if (argc != 2) {
                printf("Usage: %s filename\n", argv[0]);
                return EXIT_FAILURE;
        }

        filename = argv[1];
        lock = create_lock(filename);
        if (lock == NULL) {
                err = MALLOC_ERR;
                goto out;
        }

        while (1) {
                err = lock_file(lock);
                if (err) {
                        goto out;
                }

                file = fopen(filename, "a+");
                fprintf(file, "%d\n", lock->pid);
                sleep(1);
                fclose(file);

                err = unlock_file(lock);
                if (err) {
                        goto out;
                }

        }
        
out:
        destroy_lock(lock);
        if (err) {
                printf("%s\n", lock_error_stringify(err));
                goto out;
        }

        return EXIT_SUCCESS;
}