#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <sys/un.h>

#define member_size(type, member) sizeof(((type *)0)->member)

char *config_read_server_name(const char *config_name)
{
        FILE *config_fp = NULL;
        char line[member_size(struct sockaddr_un, sun_path)] = {0};
        char *server_name = NULL;

        if (access(config_name, F_OK) != 0) {
                return NULL;
        }

        config_fp = fopen(config_name, "r");
        if (config_fp == NULL) {
                return NULL;
        }

        if (fgets(line, sizeof(line), config_fp) == NULL) {
                goto conf_read_out;
        }

        line[strcspn(line, "\n\r")] = '\0';
        if (strlen(line) == 0) {
                goto conf_read_out;
        }

        server_name = calloc(1, sizeof(char) * (strlen(line) + sizeof("/tmp/")));
        if (server_name == NULL) {
                goto conf_read_out;
        }
        sprintf(server_name, "/tmp/%s", line);

conf_read_out:
        fclose(config_fp);
        return server_name;
}