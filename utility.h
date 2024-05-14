#ifndef UTILITY_H
#define UTILITY_H

// void read_command_line(int argc, char **argv, char *username, char *password, char *folder, char *command, char *server_name, char *mesg_num, int tls);
void toUpperCase(char *str);
char* generate_tag();
void print_headers(const char *overview);
int get_total_message(const char *folder, int sockfd);
int valid_message_number(const char *mesg_num);
int valid_server_name(const char *server_name);
int valid_ip(const char *address);
int valid_folder_name(const char *folder);
int check_mime_header(const char *part, int mode);
#endif // UTILITY_H