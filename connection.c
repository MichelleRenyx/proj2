#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <sys/types.h>
#include "utility.h"
#include "connection.h"
#include "commands.h"

#define PORT_IMAP 143
#define PORT_IMAPS 993

int connect_login(const char *server_name, const char *username, const char *password, int tls) {
    int port = tls ? PORT_IMAPS : PORT_IMAP;
    struct sockaddr_in server_addr;
    int sockfd;
    struct hostent *he;
    char login_command[256];
    char buffer[1024];

    if ((he = gethostbyname(server_name)) == NULL) {
        herror("gethostbyname");
        exit(1);
    }

    // Create socket
    if ((sockfd = socket(AF_INET, SOCK_STREAM, 0)) == -1) {
        perror("socket");
        exit(1);
    }
    // Setup server address
    memset(&server_addr, 0, sizeof(server_addr));
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(port);
    memcpy(&server_addr.sin_addr, he->h_addr_list[0], he->h_length);

    // Connect to the server
    if (connect(sockfd, (struct sockaddr*)&server_addr, sizeof(server_addr)) == -1) {
        perror("connect");
        close(sockfd);
        exit(2);
    }

    // Send LOGIN command
    sprintf(login_command, "1 LOGIN %s %s\r\n", username, password);
    if (send(sockfd, login_command, strlen(login_command), 0) == -1) {
        perror("send");
        close(sockfd);
        exit(2);
    }

    // Read response
    if (recv(sockfd, buffer, sizeof(buffer), 0) <= 0) {
        perror("recv");
        close(sockfd);
        exit(2);
    }
    if (strstr(buffer, "NO") != NULL) {
        fprintf(stderr, "Login failure\n");
        exit(3);
    }

    return sockfd;
}


void safe_disconnect(int sockfd) {
    char logout[] = "1 LOGOUT\r\n";
    send(sockfd, logout, strlen(logout), 0);
    shutdown(sockfd, SHUT_RDWR);
    close(sockfd);
}