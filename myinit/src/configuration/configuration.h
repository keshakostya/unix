#ifndef __CONFIG_H
#define __CONFIG_H

#include <stddef.h>

struct ConfigurationNode {
        struct ConfigurationNode *next;
        char *line;
};

struct Configuration {
        struct ConfigurationNode *head;
        size_t len;
};

struct Configuration *configuration_read(const char *config_path);

void configuration_destroy(struct Configuration *config);

#endif /* __CONFIG_H */