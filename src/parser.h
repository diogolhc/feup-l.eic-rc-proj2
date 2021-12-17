typedef struct parsed_params {
    char *user;
    char *password;
    char *host;
    char *url_path;
} parsed_params_t;

parsed_params_t* parse_input_params(const char* input);

void delete_parsed_params(parsed_params_t *parsed_params);
