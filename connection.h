#ifndef CONNECTION_H
#define CONNECTION_H

#define IMAP_PORT 143
#define IMAP_SSL_PORT 993


void send_command(SSL *ssl, const char *command);
char* receive_response(SSL *ssl);


#endif
