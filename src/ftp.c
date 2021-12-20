#include "ftp.h"
#include "ftp_return_codes.h"

#include <stdio.h>
#include <netdb.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <string.h>
#include <strings.h>
#include <unistd.h>
#include <stdlib.h>
#include <stdbool.h>
#include <math.h>

#define RECV_BUFFER_START_SIZE 1000


static char *get_ip(char *host_name) {
    if (host_name == NULL) {
        return NULL;
    }

    struct hostent *h = NULL;

    // TODO maybe use getaddrinfo(3) instead (not obsolete)
    if ((h = gethostbyname(host_name)) == NULL) {
        herror("gethostbyname()");
        return NULL;
    }

    return inet_ntoa(*((struct in_addr *) h->h_addr));
}

static int connect_to_host(char *host_ip, uint16_t port) {
    int sockfd;

    struct sockaddr_in server_addr;
    bzero((char *) &server_addr, sizeof(server_addr));
    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = inet_addr(host_ip);
    server_addr.sin_port = htons(port);

    if ((sockfd = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
        perror("socket()");
        return -1;
    }
    
    if (connect(sockfd,
                (struct sockaddr *) &server_addr,
                sizeof(server_addr)) < 0) {
        perror("connect()");
        return -1;
    }

    return sockfd;
}

// *recv_buffer needs to be free() outside after successfull return
static int ftp_read_line(int socket_fd, char **recv_buffer) {
    if (recv_buffer == NULL) {
        return -1;
    }

    size_t buffer_size = RECV_BUFFER_START_SIZE * sizeof(char);
    char *buffer = malloc(buffer_size);
    if (buffer == NULL) {
        return -1;
    }

    int index = 0;
    while (true) {
        int res = recv(socket_fd, &buffer[index], 1, 0);

        if (res == -1) {
            return -1;
        } else if (res == 0) {
            // TODO what to do?
            printf("FTP connection closed already.\n");
            return -1;
        }

        if (index > 0 && buffer[index-1] == '\r' && buffer[index] == '\n') {
            buffer[index+1] = '\0';
            break;
        }

        index += 1;
        if (index+1 >= buffer_size) { // index+1 so that a '\0' can be added after
            buffer_size += RECV_BUFFER_START_SIZE;
            char *new_buffer = realloc(buffer, buffer_size);
            if (new_buffer == NULL) {
                free(buffer);
                return -1;
            }

            buffer = new_buffer;
        }
    }

    *recv_buffer = buffer;
    return 0;
}

static int get_num_length(int num) {
    return ceil(log10((double)num));
}

static int ftp_read_response(int socket_fd) {
    if (socket_fd < 0) {
        return -1;
    }

    int response_code = -1;
    bool last_line_received = false;
    while (!last_line_received) {
        char *line_received = NULL;

        if (ftp_read_line(socket_fd, &line_received) != 0) {
            return -1;
        }

        printf("RECV: %s", line_received); // << DEBUG

        response_code = atoi(line_received);
        int resp_num_digits = get_num_length(response_code);
        if (line_received[resp_num_digits] == ' ') {
            last_line_received = true;
        }

        free(line_received);
        line_received = NULL;
    }

    return response_code;
}

int ftp_setup(char *host_name) {
    if (host_name == NULL) {
        return -1;
    }

    char *host_ip = get_ip(host_name);
    if (host_ip == NULL) {
        return -1;
    }

    int socket_fd_command = connect_to_host(host_ip, FTP_COMMAND_PORT);
    if (socket_fd_command < 0) {
        return -1;
    }

    if (ftp_read_response(socket_fd_command) != FTP_CODE_SERVICE_READY_FOR_NEW_USER) {
        return -1;
    }

    return socket_fd_command;
}

static int ftp_send_command(int socket_fd, char *command, char *arg) {
    if (command == NULL || arg == NULL) {
        return -1;
    }

    int cmd_size = snprintf(NULL, 0, "%s %s\r\n",
                                command, arg);  
    
    if (cmd_size == -1) {
        return -1;
    }

    char *cmd = malloc(cmd_size + 1); // +1 for '\0'

    if (cmd == NULL) {
        return -1;
    }

    if (snprintf(cmd, cmd_size + 1, "%s %s\r\n",
        command, arg) < 0 ) {
        free(cmd);
        return -1;
    }

    printf("SENT: %s", cmd); // << DEBUG
    if (send(socket_fd, cmd, cmd_size, 0) != cmd_size) {
        free(cmd);
        return -1;
    }

    free(cmd);
    return 0;
}

int ftp_login(int socket_fd, char *user, char *pass) {
    if (user == NULL || pass == NULL) {
        return -1;
    }

    if (ftp_send_command(socket_fd, "USER", user) != 0) {
        return -1;
    }

    if (ftp_read_response(socket_fd) != FTP_CODE_USER_NAME_OKAY_NEED_PASSWORD) {
        // TODO should we check for other returns? there are many...
        return -1;
    }

    if (ftp_send_command(socket_fd, "PASS", pass) != 0) {
        return -1;
    }

    if (ftp_read_response(socket_fd) != FTP_CODE_LOGIN_SUCCESSFUL) {
        // TODO should we check for other returns? there are many...
        return -1;
    }

    return 0;
}

int ftp_download_file(int socket_fd, char *path) {
    // TODO
    return 0;
}

int ftp_close(int socket_fd) {
    ftp_send_command(socket_fd, "QUIT", "");
    if (ftp_read_response(socket_fd) != FTP_CODE_SERVICE_CLOSING_CONTROL_CONNECTION) {
        // TODO what to do?
    }
    close(socket_fd);
    return 0;
}
