#ifndef COMMANDS_H
#define COMMANDS_H

#include <openssl/ssl.h>

void send_login_command(SSL *ssl, const char *username, const char *password);
char* fetch_email(SSL *ssl, const char *command);

#endif
