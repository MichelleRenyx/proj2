#ifndef COMMANDS_H
#define COMMANDS_H

#include <openssl/ssl.h>
#include <openssl/err.h>

// #define IMAP_PORT 143
// #define IMAP_SSL_PORT 993

SSL *create_socket(const char *hostname, int port, int use_tls);
SSL_CTX* init_ssl_context(void);

void send_login_command(SSL *ssl, const char *username, const char *password);
char* fetch_email(SSL *ssl, const char *command);

#endif
