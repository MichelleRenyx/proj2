#ifndef COMMANDS_H
#define COMMANDS_H

#include <stdio.h>

#define IMAP_PORT 143
#define IMAP_SSL_PORT 993

ssize_t read_line(int sockfd, char *buffer, size_t max_len);
ssize_t read_bytes(int sockfd, char *buffer, size_t num_bytes);
void retrieve(const char *server_name, const char *username, const char *password, const char *folder, const char *mesg_num, int tls);
void parse(const char *server_name, const char *username, const char *password, const char *folder, const char *mesg_num, int tls);
void mime(const char *server_name, const char *username, const char *password, const char *folder, const char *mesg_num, int tls);
void list(const char *server_name, const char *username, const char *password, const char *folder, int tls);


#endif