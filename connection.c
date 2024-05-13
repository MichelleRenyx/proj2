#include "commands.h"
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>

void send_command(SSL *ssl, const char *command) {
    if (SSL_write(ssl, command, strlen(command)) <= 0) {
        ERR_print_errors_fp(stderr);
        exit(EXIT_FAILURE);
    }
}

char* receive_response(SSL *ssl) {
    char *response = malloc(4096);
    if (!response) {
        fprintf(stderr, "Memory allocation failed\n");
        return NULL;  // 更改为返回 NULL
    }

    int nbytes = SSL_read(ssl, response, 4096 - 1);
    if (nbytes < 0) {
        ERR_print_errors_fp(stderr);
        free(response);
        return NULL;  // 更改为返回 NULL
    }

    response[nbytes] = '\0';
    return response;
}
