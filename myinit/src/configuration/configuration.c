#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "configuration.h"

struct Configuration *configuration_read(const char *config_path)
{
        struct Configuration *config = NULL;
        struct ConfigurationNode *node = NULL;
        FILE *config_file = NULL;
        char line[1024] = {0};

        if (!config_path) {
                return NULL;
        }

        config_file = fopen(config_path, "r");
        if (!config_file) {
                return NULL;
        }

        config = calloc(1, sizeof(struct Configuration));
        if (!config) {
                goto err_close_file;
        }

        while(fgets(line, sizeof(line), config_file)) {
                line[strcspn(line, "\n\r")] = '\0';
                if (!strlen(line)) {
                        continue;
                }

                node = malloc(sizeof(struct ConfigurationNode));
                if (!node) {
                        goto err_free_config;
                }

                node->line = strdup(line);
                if (config->head) {
                        node->next = config->head;
                } else {
                        node->next = NULL;
                }
                config->head = node;
                config->len++;
        }

        fclose(config_file);
        return config;
err_free_config:
        configuration_destroy(config);
err_close_file:
        fclose(config_file);
        return NULL;
}

void configuration_destroy(struct Configuration *config)
{
        struct ConfigurationNode *n = NULL;
        struct ConfigurationNode *nn = NULL;
        if (!config) {
                return;
        }

        n = nn = config->head;
        while (n) {
                nn = n->next;
                free(n->line);
                free(n);
                n = nn;
        }

        free(config);
}