#include "connection.h"
#include "commands.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int main(int argc, char *argv[]) {
    if (argc < 6) {
        fprintf(stderr, "Usage: %s <hostname> <username> <password> <command> <tls>\n", argv[0]);
        exit(EXIT_FAILURE);
    }

    const char *hostname = argv[1];
    const char *username = argv[2];
    const char *password = argv[3];
    const char *command = argv[4];
    int use_tls = atoi(argv[5]); // simplistic way to handle boolean

    int port = use_tls ? IMAP_SSL_PORT : IMAP_PORT;
    int sockfd = create_socket(hostname, port);
    if (use_tls) {
        sockfd = upgrade_to_tls(sockfd);
    }

    char login_command[1024];
    snprintf(login_command, sizeof(login_command), "LOGIN %s %s\r\n", username, password);
    send_command(sockfd, login_command);

    char *response = receive_response(sockfd);
    printf("Server response: %s\n", response);
    free(response);

    // Assuming command handling and further actions are based on `command` input
    return 0;
}

