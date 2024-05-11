/*
Create and manage socket connections.
Login and logout functionality.
Error handling and status management.
*/
/*
Create and manage socket connections.
Login and logout functionality.
Error handling and status management.
*/
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <openssl/ssl.h>
#include <openssl/err.h>

#define IMAP_PORT 143
#define IMAP_SSL_PORT 993

// 函数声明
int create_socket(char *hostname, int port);
SSL_CTX* init_ssl_context(void);
void cleanup(SSL_CTX *ctx, int sock, SSL *ssl);
SSL* create_ssl_connection(char *hostname, int port, SSL_CTX *ctx);

// 创建与IMAP服务器的连接
int create_socket(char *hostname, int port) {
    int sockfd;
    struct sockaddr_in server_addr;

    // 创建socket
    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd < 0) {
        perror("Error creating socket");
        return -1;
    }

    // 设置服务器地址
    memset(&server_addr, 0, sizeof(server_addr));
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(port);
    if (inet_pton(AF_INET, hostname, &server_addr.sin_addr) <= 0) {
        perror("Invalid address/ Address not supported");
        close(sockfd);
        return -1;
    }

    // 连接到服务器
    if (connect(sockfd, (struct sockaddr *)&server_addr, sizeof(server_addr)) < 0) {
        perror("Connection Failed");
        close(sockfd);
        return -1;
    }
    return sockfd;
}

// 初始化SSL上下文
SSL_CTX* init_ssl_context(void) {
    SSL_CTX *ctx;

    SSL_library_init();
    OpenSSL_add_all_algorithms();
    SSL_load_error_strings();
    ctx = SSL_CTX_new(SSLv23_client_method());
    if (ctx == NULL) {
        ERR_print_errors_fp(stderr);
        exit(EXIT_FAILURE);
    }
    return ctx;
}

// 创建SSL连接
SSL* create_ssl_connection(char *hostname, int port, SSL_CTX *ctx) {
    int sock = create_socket(hostname, port);
    SSL *ssl = SSL_new(ctx);
    SSL_set_fd(ssl, sock);
    if (SSL_connect(ssl) != 1) {
        ERR_print_errors_fp(stderr);
        SSL_free(ssl);
        close(sock);
        SSL_CTX_free(ctx);
        exit(EXIT_FAILURE);
    }
    return ssl;
}

// 清理资源
void cleanup(SSL_CTX *ctx, int sock, SSL *ssl) {
    if (ssl) {
        SSL_shutdown(ssl);
        SSL_free(ssl);
    }
    close(sock);
    SSL_CTX_free(ctx);
}

int main(int argc, char **argv) {
    SSL_CTX *ctx;
    SSL *ssl;

    // 初始化SSL库和创建SSL上下文
    ctx = init_ssl_context();

    // 创建SSL连接到IMAP服务器
    ssl = create_ssl_connection("imap.example.com", IMAP_SSL_PORT, ctx);

    // 登录、发送命令和处理响应的代码可以在此处继续添加
    // 示例代码，具体实现根据需求编写

    // 清理资源
    cleanup(ctx, SSL_get_fd(ssl), ssl);
    return 0;
}
