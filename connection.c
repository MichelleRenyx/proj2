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
#include "commands.h"



#define PORT_IMAP 143
#define PORT_IMAPS 993
#define BUFFER_SIZE 1024
//#define CONNECT_TIMEOUT 10 // 设置连接超时为10秒
//#define BUFFER_SIZE 4*1024*1024
//int connect_login(const char *server_name, const char *username, const char *password, int tls) {

//    int port = tls ? PORT_IMAPS : PORT_IMAP;
//    struct sockaddr_in server_addr;
//    int sockfd;
//    struct hostent *he;
//    char login_command[256];
//    char buffer[BUFFER_SIZE];
//
//    if ((he = gethostbyname(server_name)) == NULL) {
//        herror("gethostbyname");
//        exit(1);
//    } else if (he->h_addr_list == NULL || he->h_length <= 0) {exit (3);};
//
//    // Create socket
//    if ((sockfd = socket(AF_INET, SOCK_STREAM, 0)) == -1) {
//        perror("socket");
//        exit(1);
//    }
//    // Setup server address
//    memset(&server_addr, 0, sizeof(server_addr));
//    server_addr.sin_family = AF_INET;
//    server_addr.sin_port = htons(port);
//    memcpy(&server_addr.sin_addr, he->h_addr_list[0], he->h_length);
//
//    // Connect to the server
//    if (connect(sockfd, (struct sockaddr*)&server_addr, sizeof(server_addr)) == -1) {
//        perror("connect");
//        // close(sockfd);
//        exit(3);
//    }
//
//    // Send LOGIN command
//    sprintf(login_command, "1 LOGIN %s %s\r\n", username, password);
//    if (send(sockfd, login_command, strlen(login_command), 0) == -1) {
//        perror("send");
//        close(sockfd);
//        exit(2);
//    }
//
//    // Read response
//    if (recv(sockfd, buffer, sizeof(buffer), 0) <= 0) {
//        perror("recv");
//        close(sockfd);
//        exit(2);
//    }
//
//
////    // Read response Refactor by readline
////    if (read_line(sockfd, buffer, sizeof(buffer)) <= 0) {
////        perror("recv");
////        close(sockfd);
////        exit(2);
////    }
//    // printf("connect_login 都打牌%s\n", buffer);
//    if (strstr(buffer, "NO") != NULL) {
//        fprintf(stderr, "Login failure\n");
//        exit(3);
//    }
//    read_line(sockfd, buffer, sizeof(buffer));
//    // printf("Login success\n");
//    return sockfd;
//}
//int connect_login(const char *hostname, int port) {
//    struct addrinfo hints, *res, *result;
//    int sockfd;
//    char port_str[6];  // 用于存储端口号的字符串表示
//
//    memset(&hints, 0, sizeof(hints));
//    hints.ai_family = AF_UNSPEC; // 支持 IPv4 或 IPv6
//    hints.ai_socktype = SOCK_STREAM;
//
//    snprintf(port_str, sizeof(port_str), "%d", port); // 将端口号转换为字符串
//    if (getaddrinfo(hostname, port_str, &hints, &result) != 0) {
//        perror("getaddrinfo failed");
//        return -1;
//    }
//
//    for (res = result; res != NULL; res = res->ai_next) {
//        sockfd = socket(res->ai_family, res->ai_socktype, res->ai_protocol);
//        if (sockfd == -1)
//            continue;
//
//        if (connect(sockfd, res->ai_addr, res->ai_addrlen) == 0)
//            break; // 成功连接
//
//        close(sockfd);
//    }
//
//    freeaddrinfo(result);
//    if (res == NULL) { // 没有成功连接
//        perror("Could not connect");
//        return -1;
//    }
//
//    return sockfd;
//}

int connect_login(const char *server_name, int tls) {
//    int port = tls ? PORT_IMAPS : PORT_IMAP;
    struct addrinfo hints, *res;
    int sockfd;
    memset(&hints, 0, sizeof(hints));
    hints.ai_family = AF_UNSPEC;
    hints.ai_socktype = SOCK_STREAM;

    char port_str[6];
    snprintf(port_str, sizeof(port_str), "%d", tls ? PORT_IMAPS : PORT_IMAP);

    if (getaddrinfo(server_name, port_str, &hints, &res) != 0) {
        fprintf(stderr, "Cannot resolve hostname %s\n", server_name);
        exit(3);
    }

    sockfd = socket(res->ai_family, res->ai_socktype, res->ai_protocol);
    if (sockfd < 0) {
        perror("Error opening socket");
        freeaddrinfo(res);
        exit(3);
    }

    if (connect(sockfd, res->ai_addr, res->ai_addrlen) < 0) {
        perror("Error connecting to server");
        //close(sockfd);
        freeaddrinfo(res);
        exit(3);
    }

    // Clean up and return the socket descriptor
    freeaddrinfo(res);

    return sockfd;
}

void safe_disconnect(int sockfd) {
    char logout[] = "1 LOGOUT\r\n";
    send(sockfd, logout, strlen(logout), 0);
    shutdown(sockfd, SHUT_RDWR);
    close(sockfd);
}