#ifndef CONNECTION_H
#define CONNECTION_H

#define IMAP_PORT 143
#define IMAP_SSL_PORT 993

int create_socket(const char *hostname, int port);
int upgrade_to_tls(int sockfd);
#endif
