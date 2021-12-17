#include "parser.h"

#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <regex.h>
#include <string.h>

static const char *REGULAR_EXPRESSION = "ftp://((.+):(.+)@)?([^/@:]+)(/[^\f\n\r\t\v\x20]*)";

#define RE_NUM_CAPTURES 6
#define RE_USER 2
#define RE_PASSWORD 3
#define RE_HOST 4
#define RE_URL_PATH 5

/*
Eg.: ftp://user:pass@ftp.up.pt/pub/kodi/timestamp.txt
0 -> ftp://user:pass@ftp.up.pt/pub/kodi/timestamp.txt
1 -> user:pass@
2 -> user
3 -> pass
4 -> ftp.up.pt
5 -> /pub/kodi/timestamp.txt
*/

static char* get_capture(const char* str, const regmatch_t *pmatch, uint8_t index) {
    if (pmatch == NULL) {
        return NULL;
    }

    regoff_t len = pmatch[index].rm_eo - pmatch[index].rm_so;

    char *captured_string = malloc((len + 1) * sizeof(char));
    if (captured_string == NULL) {
        return NULL;
    }

    strncpy(captured_string, str + pmatch[index].rm_so, len);
    captured_string[len] = '\0';


    return captured_string;
}

parsed_params_t* parse_input_params(const char* input) {
    regex_t     regex;
    regmatch_t  pmatch[RE_NUM_CAPTURES];

    if (regcomp(&regex, REGULAR_EXPRESSION, REG_EXTENDED) != 0) {
        return NULL;
    }

    if (regexec(&regex, input, RE_NUM_CAPTURES, pmatch, 0) != 0) {
        regfree(&regex);
        return NULL;
    }

    regfree(&regex);

    parsed_params_t * parsed_params = malloc(sizeof(parsed_params_t));
    parsed_params->user     = get_capture(input, pmatch, RE_USER);
    parsed_params->password = get_capture(input, pmatch, RE_PASSWORD);
    parsed_params->host     = get_capture(input, pmatch, RE_HOST);
    parsed_params->url_path = get_capture(input, pmatch, RE_URL_PATH);

    return parsed_params;
}

void delete_parsed_params(parsed_params_t *parsed_params) {
    if (parsed_params == NULL) {
        return;
    }

    if (parsed_params->host)        free(parsed_params->user);
    if (parsed_params->password)    free(parsed_params->password);
    if (parsed_params->host)        free(parsed_params->host);
    if (parsed_params->url_path)    free(parsed_params->url_path);
}
