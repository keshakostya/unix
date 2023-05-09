#ifndef __ERROR_H
#define __ERROR_H

enum MyinitError {
        OK = 0,
        BASE_ERR = -1000,
        LOG_FILE_OPEN_ERR,
        CONFIG_READ_ERR,
        TASKS_CREATE_ERR,
};

char *error_stringify(int err);

#endif /* __ERROR_H */