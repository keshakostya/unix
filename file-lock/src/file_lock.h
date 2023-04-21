#ifndef __FILE_LOCK_H
#define __FILE_LOCK_H

struct FileLock {
        char *lock_filename;
        int lock_fd;
        int pid;
        char pid_str[10];
};

typedef enum {
        BASE_ERR = -1000,
        MALLOC_ERR,
        READ_ERR,
        WRITE_ERR,
        IO_ERR,
        RACE_CONDITION_ERR,
        FILE_NOT_LOCKED_ERR,
        LOCK_FILE_REMOVE_ERR,
        LOCK_NOT_SET_UP_ERR,
} lock_err;

struct FileLock *create_lock(const char *filename);

int lock_file(struct FileLock *lock);

int unlock_file(struct FileLock *lock);

void destroy_lock(struct FileLock *lock);

char *lock_error_stringify(int error_code);

#endif /* __FILE_LOCK_H */