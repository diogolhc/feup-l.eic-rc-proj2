#include <stdio.h>

#include "parser.h"
#include "ftp.h"

int main(int argc, char *argv[]) {
    if (argc != 2) {
        printf("Usage:\n%s ftp://[<user>:<password>@]<host>/<url-path>\n", argv[0]);
        return -1;
    }

    parsed_params_t* parsed_params = parse_input_params(argv[1]);
    if (parsed_params == NULL) {
        printf("Invalid Input\n");
        printf("Usage:\n%s ftp://[<user>:<password>@]<host>/<url-path>\n", argv[0]);
        return -1;
    }

    printf("USER: \"%s\"\n", parsed_params->user);
    printf("PASSWORD: \"%s\"\n", parsed_params->password);
    printf("HOST: \"%s\"\n", parsed_params->host);
    printf("URL-PATH: \"%s\"\n", parsed_params->url_path);

    int socket_fd = -1;
    if ((socket_fd = ftp_setup(parsed_params->host)) < 0) {
        delete_parsed_params(parsed_params);
        return -1;
    }

    if (ftp_login(socket_fd, parsed_params->user, parsed_params->password) != 0) {
        delete_parsed_params(parsed_params);
        return -1;
    }

    if (ftp_download_file(socket_fd, parsed_params->url_path) != 0) {
        delete_parsed_params(parsed_params);
        return -1;
    }

    ftp_close(socket_fd);
    delete_parsed_params(parsed_params);
    return 0;
}
