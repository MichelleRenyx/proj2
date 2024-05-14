#include "utility.h"
#include "connection.h"
#include "commands.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>



#define PORT_IMAP 143
#define PORT_IMAPS 993
#define CONNECT_TIMEOUT 10 // 设置连接超时为10秒
#define BUFFER_SIZE 4*1024*1024
int connect_login(const char *server_name, const char *username, const char *password, int tls) {
    int port = tls ? PORT_IMAPS : PORT_IMAP;
    struct sockaddr_in server_addr;
    int sockfd;
    struct hostent *he;
    char login_command[256];
    char buffer[BUFFER_SIZE];

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
    // printf("Login success\n");
    return sockfd;
}

// int connect_login(const char *server_name, const char *username, const char *password, int tls) {
//     int port = tls ? PORT_IMAPS : PORT_IMAP;
//     struct addrinfo hints, *res, *p;
//     int sockfd;
//     char host[256];
//     sprintf(host, "%s:%d", server_name, port);

//     memset(&hints, 0, sizeof(hints));
//     hints.ai_family = AF_UNSPEC;
//     hints.ai_socktype = SOCK_STREAM;

//     if (getaddrinfo(server_name, port, &hints, &res) != 0) {
//         perror("getaddrinfo");
//         exit(1);
//     }

//     for (p = res; p != NULL; p = p->ai_next) {
//         if ((sockfd = socket(p->ai_family, p->ai_socktype, p->ai_protocol)) == -1) {
//             continue;
//         }

//         if (connect(sockfd, p->ai_addr, p->ai_addrlen) == -1) {
//             close(sockfd);
//             continue;
//         }

//         break;
//     }

//     if (p == NULL) {
//         fprintf(stderr, "Failed to connect\n");
//         exit(2);
//     }

//     freeaddrinfo(res);

//     char login_command[256];
//     sprintf(login_command, "1 LOGIN %s %s\r\n", username, password);
//     send(sockfd, login_command, strlen(login_command), 0);

//     // Read response
//     char buffer[1024];
//     recv(sockfd, buffer, sizeof(buffer), 0);
//     if (strstr(buffer, "NO") != NULL) {
//         fprintf(stderr, "Login failure\n");
//         exit(3);
//     }

//     return sockfd;
// }

void safe_disconnect(int sockfd) {
    char logout[] = "1 LOGOUT\r\n";
    send(sockfd, logout, strlen(logout), 0);
    shutdown(sockfd, SHUT_RDWR);
    close(sockfd);
}