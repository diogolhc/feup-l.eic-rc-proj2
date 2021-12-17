#include <stdio.h>

#include "parser.h"
#include "defines.h"


int main(int argc, char *argv[]) {
    if (argc != 2) {
        printf("Invalid number of parameters.\n");
        return -1;
    }

    parsed_params_t* parsed_params = parse_input_params(argv[1]);
    if (parsed_params == NULL) {
        printf("Invalid Input\n");
        return -1;
    }

    printf("USER: \"%s\"\n", parsed_params->user);
    printf("PASSWORD: \"%s\"\n", parsed_params->password);
    printf("HOST: \"%s\"\n", parsed_params->host);
    printf("URL-PATH: \"%s\"\n", parsed_params->url_path);

    delete_parsed_params(parsed_params);
    return 0;
}
