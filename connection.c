#include "commands.h"
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>

void send_command(int sockfd, const char *command) {
    int len = strlen(command);
    if (write(sockfd, command, len) != len) {
        perror("Failed to write to socket");
        exit(EXIT_FAILURE);
    }
}

char* receive_response(int sockfd) {
    char *response = malloc(4096); // allocate buffer
    if (response == NULL) {
        fprintf(stderr, "Memory allocation failed\n");
        exit(EXIT_FAILURE);
    }

    int nbytes = read(sockfd, response, 4096);
    if (nbytes < 0) {
        perror("Failed to read from socket");
        free(response);
        exit(EXIT_FAILURE);
    }

    response[nbytes] = '\0';
    return response;
}
