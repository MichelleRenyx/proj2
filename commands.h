#ifndef COMMANDS_H
#define COMMANDS_H
#define IMAP_PORT 143
#define IMAP_SSL_PORT 993

#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>  // 对于 close()


//int create_socket(const char *hostname, int port);  // 移除 use_tls 参数，更新返回类型
void retrive(int sockfd, const char *messageNum);
void login_imap(int sockfd, const char *username, const char *password);
void select_folder(int sockfd, const char *folder);
void parse_email_headers(int sockfd, const char *messageNum);
void decode_mime_message(int sockfd, const char *messageNum);
void list_email_subjects(int sockfd);
void send_command(int sockfd, const char *command);
char* receive_response(int sockfd);

#endif
