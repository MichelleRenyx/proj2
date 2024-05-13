#ifndef COMMANDS_H
#define COMMANDS_H

#include <openssl/ssl.h>
#include <openssl/err.h>

// #define IMAP_PORT 143
// #define IMAP_SSL_PORT 993

SSL* create_socket(const char *hostname, int port, int use_tls);
SSL_CTX* init_ssl_context(void);

void send_login_command(SSL *ssl, const char *username, const char *password);
char* fetch_email(SSL *ssl, const char *command);
void login_imap(SSL *ssl, const char *username, const char *password);
void select_folder(SSL *ssl, const char *folder);
void parse_email_headers(SSL *ssl, const char *messageNum);
void decode_mime_message(SSL *ssl, const char *messageNum);
void list_email_subjects(SSL *ssl);

#endif
