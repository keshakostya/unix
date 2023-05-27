#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <unistd.h>
#include <stdbool.h>
#include <time.h>
#include "config.h"

int random_number(int min, int max) 
{
    int range = (max - min); 
    int div = RAND_MAX / range;
    return min + (rand() / div);
}

int delay_client(double delay)
{
        FILE *dev_null = NULL;
        int rand_bytes_cnt = 0;

        dev_null = fopen("/dev/null", "r");
        if (dev_null == NULL) {
                fprintf(stderr, "Fail to set random delay\n");
                return -1;
        }

        rand_bytes_cnt = random_number(1, 255);
        while (rand_bytes_cnt)
        {
                fgetc(dev_null);
                rand_bytes_cnt--;
        }
        sleep(delay);
        fclose(dev_null);
        return 0;
}

int setup_client(char *server_addr)
{
        int client_fd = -1;
        struct sockaddr_un addr;
        int err = 0;

        client_fd = socket(AF_UNIX, SOCK_STREAM, 0);
        if (client_fd == -1) {
                fprintf(stderr, "Fail to create client socket\n");
                return -1;
        }

        memset(&addr, 0, sizeof(struct sockaddr_un));
        addr.sun_family = AF_UNIX;
        strcpy(addr.sun_path, server_addr);

        err = connect(client_fd, (struct sockaddr *) &addr, sizeof(struct sockaddr_un));
        if (err == -1) {
                close(client_fd);
                fprintf(stderr, "Fail to connect to server at %s\n", server_addr);
                return -1;
        }

        return client_fd;
}

void client_start(char *server_addr, double delay)
{
        int client_fd = -1;
        char line[50] = {0};
        

        client_fd = setup_client(server_addr);
        if (client_fd == -1) {
                return;
        }

        while (true)
        {
                if (fgets(line, sizeof(line), stdin) == NULL) {
                        break;
                }

                line[strcspn(line, "\n\r")] = '\0';
                if (strlen(line) == 0) {
                        break;
                }

                if (send(client_fd, line, strlen(line) + 1, 0) <= 0) {
                        fprintf(stderr, "Fail to send number to server\n");
                        break;
                }

                if (recv(client_fd, line, sizeof(line), 0) <= 0) {
                        fprintf(stderr, "Fail to recv server state\n");
                        break;
                }

                printf("Current server state %s\n", line);
                if (delay != 0.0) {
                        if (delay_client(delay) != 0) {
                                fprintf(stderr, "Fail to set delay\n");
                                break;
                        }
                }
        }

        close(client_fd);
}

int main(int argc, char **argv)
{
        char *progname = argv[0];
        char *server_addr = NULL;
        double delay = 0.0;
        time_t begin = 0;
        time_t end = 0;
        double prog_time = 0.0;
        
        if (argc < 3 || argc > 4) {
                fprintf(stderr, "Usage: %s CONF_PATH CLIENT_ID [DELAY]\n", progname);
                return EXIT_SUCCESS;
        }

        if (argc == 4) {
                delay = atof(argv[3]);
        }

        server_addr = config_read_server_name(argv[1]);
        if (server_addr == NULL) {
                return EXIT_FAILURE;
        }

        srand(time(NULL));
        begin = time(0);
        client_start(server_addr, delay);
        end = time(0);

        prog_time = difftime(end, begin);
        printf("Client %s run in %f seconds\n", argv[2], prog_time);
        
        return EXIT_SUCCESS;
}