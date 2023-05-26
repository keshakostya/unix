#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <sys/select.h>
#include <errno.h>
#include <stdbool.h>
#include "config.h"
#include "logger.h"

#define SERVER_LOG_FILENAME "/tmp/server.log"
#define MAX_CONNECTIONS 128
#define SOCK_BUFF_SIZE 50

FILE *log_file = NULL;
static int SERVER_STATE = 0;

int setup_server(char *server_name)
{
        int sock_fd = -1;
        int err = 0;
        int optval = 0;
        struct sockaddr_un addr;

        err = unlink(server_name);
        if (err == -1 && errno != ENOENT) {
                fprintf(stderr, "Fail to delete socket name\n");
                return -1;
        }

        sock_fd = socket(AF_UNIX, SOCK_STREAM, 0);
        if (sock_fd == -1) {
                fprintf(stderr, "Fail to create socket\n");
                return -1;
        }

        err = setsockopt(sock_fd, SOL_SOCKET, SO_REUSEADDR, (char *) &optval, sizeof(optval));
        if (err == -1) {
                fprintf(stderr, "Fail to set sock reusable\n");
                close(sock_fd);
                return -1;
        }

        memset(&addr, 0, sizeof(struct sockaddr_un));
        addr.sun_family = AF_UNIX;
        sprintf(addr.sun_path, "%s", server_name);

        err = bind(sock_fd, (struct sockaddr *) &addr, sizeof(struct sockaddr_un));
        if (err == -1) {
                fprintf(stderr, "Fail to bind address with socket fd\n");
                close(sock_fd);
                return -1;
        }

        return sock_fd;
}

int accept_client(int server_fd, int clients[])
{
        int client_fd = -1;
        long sbrk_value = 0;

        client_fd = accept(server_fd, NULL, NULL);
        if (client_fd == -1) {
                return -1;
        }

        for (int i = 0; i < MAX_CONNECTIONS; ++i) {
                if (clients[i] == 0) {
                        clients[i] = client_fd;
                        sbrk_value = (long) sbrk(0);
                        log_write(log_file, "Accept client %d, client sbrk %ld\n", client_fd, sbrk_value);
                        break;
                }
        }

        return 0;
}

int recv_from_client(int client_fd)
{
        char buffer[SOCK_BUFF_SIZE] = {0};
        int number = 0;

        if (recv(client_fd, buffer, sizeof(buffer), 0) <= 0) {
                return -2;
        }

        number = atoi(buffer);
        SERVER_STATE += number;

        sprintf(buffer, "%d", SERVER_STATE);
        
        if (send(client_fd, buffer, strlen(buffer) + 1, 0) <= 0) {
                fprintf(stderr, "Fail to send server state to client\n");
                return -1;
        }

        log_write(log_file, "Received number from client %d. Current server state %d\n", number, SERVER_STATE);
        return 0;
}

void server_loop(int server_fd, char *server_address)
{
        fd_set master_fd_set;
        int clients[MAX_CONNECTIONS] = {0};
        int max_sock_fd = server_fd;
        int client_fd = -1;
        int select_ret = 0;
        int err = 0;

        err = listen(server_fd, MAX_CONNECTIONS);
        if (err == -1) {
                fprintf(stderr, "Fail to start listenning\n");
                return;
        }

        log_write(log_file, "Server start listening at %s\n", server_address);

        while (true) {
                FD_ZERO(&master_fd_set);
                FD_SET(server_fd, &master_fd_set);

                for (int i = 0; i < MAX_CONNECTIONS; ++i) {
                        client_fd = clients[i];

                        if (client_fd > 0) {
                                FD_SET(client_fd, &master_fd_set);
                        }

                        if (client_fd > max_sock_fd) {
                                max_sock_fd = client_fd;
                        }
                }

                select_ret = select(max_sock_fd + 1, &master_fd_set, NULL, NULL, NULL);
                if (select_ret < 0) {
                        fprintf(stderr, "Select error\n");
                        return;
                }

                if (FD_ISSET(server_fd, &master_fd_set)) {
                        err = accept_client(server_fd, clients);
                        if (err == -1) {
                                fprintf(stderr, "Fail to accept client\n");
                                return;
                        }
                }

                for (int i = 0; i < MAX_CONNECTIONS; ++i) {
                        if (FD_ISSET(clients[i], &master_fd_set)) {
                                err = recv_from_client(clients[i]);
                                if (err == -2) {
                                        close(clients[i]);
                                        clients[i] = 0;
                                } else if (err == -1) {
                                        return;
                                }
                        }
                }
        }
}

void server_start(char *server_name)
{
        int sock_fd = -1;

        sock_fd = setup_server(server_name);
        if (sock_fd == -1) {
                log_write(log_file, "Fail to create server\n");
                return;
        }

        server_loop(sock_fd, server_name);
        close(sock_fd);
        fclose(log_file);
}

int main(int argc, char **argv)
{
        char *server_name = NULL;
        char *progname = argv[0];

        if (argc != 2) {
                fprintf(stderr, "Usage: %s CONFIG_FILE\n", progname);
                return EXIT_FAILURE;
        }

        server_name = config_read_server_name(argv[1]);
        if (server_name == NULL) {
                fprintf(stderr, "Fail to read server name from config file - %s\n", argv[1]);
                return EXIT_FAILURE;
        }

        log_file = create_log_file(SERVER_LOG_FILENAME);
        if (log_file == NULL) {
                fprintf(stderr, "Fail create log file - %s\n", SERVER_LOG_FILENAME);
                return EXIT_FAILURE;               
        }

        server_start(server_name);

        free(server_name);
        return EXIT_SUCCESS;
}