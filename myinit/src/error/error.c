#include "error.h"

char *error_stringify(int err)
{
        switch (err)
        {
        case OK:
                return "";
        case LOG_FILE_OPEN_ERR:
                return "Failed to open log file";
        case CONFIG_READ_ERR:
                return "Failed to read config";
        case TASKS_CREATE_ERR:
                return "Failed to create tasks";
        default:
                return "Unknown error";
        }
}