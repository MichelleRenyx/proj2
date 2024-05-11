// imap_connection.h
#ifndef IMAP_CONNECTION_H
#define IMAP_CONNECTION_H

#include <openssl/ssl.h>

// 定义IMAP服务器的端口
#define IMAP_PORT 143
#define IMAP_SSL_PORT 993

// 函数声明
int create_socket(char *hostname, int port);
SSL_CTX* init_ssl_context(void);
SSL* create_ssl_connection(char *hostname, int port, SSL_CTX *ctx);
void cleanup(SSL_CTX *ctx, int sock, SSL *ssl);

#endif // IMAP_CONNECTION_H
