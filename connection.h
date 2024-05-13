// #ifndef CONNECTION_H
// #define CONNECTION_H

// #define IMAP_PORT 143
// #define IMAP_SSL_PORT 993

//int connect_login(const char *server_name, const char *username, const char *password, int port);
void safe_disconnect(int sockfd);

