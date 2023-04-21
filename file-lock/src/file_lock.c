#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <error.h>
#include "file_lock.h"

#define CLOSE_FD(FD)            \
do                              \
{                               \
        close(FD);              \
        FD = -1;                \
} while (0)


struct FileLock *create_lock(const char *filename)
{
        struct FileLock *lock = NULL;
        size_t filename_len = 0;
        
        lock = malloc(sizeof(struct FileLock));
        if (lock == NULL) {
                return NULL;
        }

        filename_len = strlen(filename);
        lock->lock_filename = malloc(sizeof(char) * filename_len + 5);
        if (lock->lock_filename == NULL) {
                free(lock);
                return NULL;
        }

        sprintf(lock->lock_filename, "%s.lck", filename);

        lock->pid = getpid();
        sprintf(lock->pid_str, "%d", lock->pid);
        
        lock->lock_fd = -1;

        return lock;
}

int lock_file(struct FileLock *lock)
{
        ssize_t writen = 0;

        if (lock == NULL) {
                return LOCK_NOT_SET_UP_ERR;
        }

        while (lock->lock_fd == -1) {
                lock->lock_fd = open(lock->lock_filename,
                                     O_CREAT | O_EXCL | O_RDWR,
                                     S_IRUSR | S_IWUSR);
        }

        writen = write(lock->lock_fd, lock->pid_str, strlen(lock->pid_str) + 1);
        if (writen == -1) {
                CLOSE_FD((lock->lock_fd));
                return WRITE_ERR;
        }

        if (lseek(lock->lock_fd, 0, SEEK_SET) == -1) {
                CLOSE_FD((lock->lock_fd));
                return IO_ERR;
        }

        return 0;
}

int unlock_file(struct FileLock *lock)
{
        char buff[10] = {0};
        int pid = 0;
        int ret = 0;
        ssize_t readn = 0;

        if (lock == NULL) {
                return LOCK_NOT_SET_UP_ERR;
        }

        if (lock->lock_fd == -1) {
                return FILE_NOT_LOCKED_ERR;
        }

        readn = read(lock->lock_fd, buff, strlen(lock->pid_str) + 1);
        if (readn == -1) {
                ret = READ_ERR;
                goto out;
        }
        pid = atoi(buff);

        if (pid == 0) {
                ret = READ_ERR;
                goto out;
        } else {
                if (pid == lock->pid) {
                        if (unlink(lock->lock_filename) == -1) {
                                ret = LOCK_FILE_REMOVE_ERR;
                                goto out;
                        }
                } else {
                        ret = RACE_CONDITION_ERR;
                        goto out;
                }
        }

out:
        CLOSE_FD(lock->lock_fd);
        return ret;
}

void destroy_lock(struct FileLock *lock)
{
        if (lock == NULL) {
                return;
        }

        if (lock->lock_filename) {
                free(lock->lock_filename);
        }

        free(lock);
}

char *lock_error_stringify(int error_code)
{
        if (error_code == 0) {
                return "";
        }

        switch (error_code)
        {
        case MALLOC_ERR:
                return "Error: fail to allocate memory!";
        case READ_ERR:
                return "Error: fail to read lock file!";
        case WRITE_ERR:
                return "Error: fail to write to lock file!";
        case IO_ERR:
                return "Error: fail to i/o with lock file!";
        case RACE_CONDITION_ERR:
                return "Error: race condition occured with another process!";
        case FILE_NOT_LOCKED_ERR:
                return "Error: lock file does not exist!";
        case LOCK_FILE_REMOVE_ERR:
                return "Error: fail to remove lock file!";
        case LOCK_NOT_SET_UP_ERR:
                return "Error: lock is not set up!";
        default:
                return "Error: unknown error!";
        }
}