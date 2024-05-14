#include "utility.h"
#include "commands.h"
#include "connection.h"
#include <openssl/ssl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "utility.h"
#include "commands.h"
#include "connection.h"
#include <openssl/ssl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int main(int argc, char **argv) {
    char *username = NULL;
    char *password = NULL;
    char *folder = NULL;
    char *command = NULL;
    char *server_name = NULL;
    char *mesg_num = NULL;
    int tls = 0; 
    
    // 为指针变量分配内存空间
    username = (char *)malloc(sizeof(char) * 100); // 假设用户名最大长度为100
    password = (char *)malloc(sizeof(char) * 100); // 假设密码最大长度为100
    folder = (char *)malloc(sizeof(char) * 100); // 假设文件夹名最大长度为100
    command = (char *)malloc(sizeof(char) * 10); // 假设命令最大长度为10
    server_name = (char *)malloc(sizeof(char) * 100); // 假设服务器名最大长度为100
    mesg_num = (char *)malloc(sizeof(char) * 10); // 假设消息编号最大长度为10
    
    if (username == NULL || password == NULL || folder == NULL || command == NULL || server_name == NULL || mesg_num == NULL) {
        fprintf(stderr, "Memory allocation failed\n");
        exit(1);
    }
    
    //printf("argc: %d\n", argc);
//    read_command_line(argc, argv, username, password, folder, command, server_name, mesg_num, tls);
    
    tls = 0; 
    int folder_occurs = 0;
    for (int i = 1; i < argc; i++) {
        if (strcmp(argv[i], "-u") == 0 && i + 1 < argc) {
            
            strcpy(username, argv[++i]);
            
        } else if (strcmp(argv[i], "-p") == 0 && i + 1 < argc) {
            strcpy(password, argv[++i]);
        } else if (strcmp(argv[i], "-f") == 0 && i + 1 < argc) {
            strcpy(folder, argv[++i]);
            folder_occurs = 1;
        } else if (strcmp(argv[i], "-n") == 0 && i + 1 < argc) {
            strcpy(mesg_num, argv[++i]);
        } else if (strcmp(argv[i], "-t") == 0) {
            tls = 1;
        } else if (strcmp(argv[i], "parse") == 0 || strcmp(argv[i], "mime") == 0 || strcmp(argv[i], "retrieve") == 0 ||
                   strcmp(argv[i], "list") == 0) {
            strcpy(command, argv[i]);
        } else {
            strcpy(server_name, argv[i]);
        }
    }
    
    if (!folder_occurs) {
        strcpy(folder, "INBOX");
    }
    
    if (strlen(username) == 0 || strlen(password) == 0 || strlen(server_name) == 0 || strlen(command) == 0) {
        fprintf(stderr, "Missing one or more of (username, password, servername, command)\n");
        exit(1);
    }
    
    if (!valid_message_number(mesg_num) && strlen(mesg_num) > 0) {
        fprintf(stderr, "Invalid message number\n");
        exit(1);
    }
    
    if (!valid_server_name(server_name) && !valid_ip(server_name)) {
        fprintf(stderr, "Invalid host name\n");
        exit(1);
    }
    
    if (!valid_folder_name(folder)) {
        fprintf(stderr, "Invalid folder name\n");
        exit(1);
    }
    //print all the values read from the command line
    // printf("username: %s\n", username);
    // printf("password: %s\n", password);
    // printf("folder: %s\n", folder);
    // printf("command: %s\n", command);
    // printf("server_name: %s\n", server_name);
    // printf("mesg_num: %s\n", mesg_num);
    // printf("tls: %d\n", tls);
    if (strcmp(command, "retrieve") == 0) {
        retrieve(server_name, username, password, folder, mesg_num, tls);
        exit(0);
    } else if (strcmp(command, "parse") == 0) {
        parse(server_name, username, password, folder, mesg_num, tls);
    } else if (strcmp(command, "mime") == 0) {
        mime(server_name, username, password, folder, mesg_num, tls);
    } else if (strcmp(command, "list") == 0) {
        list(server_name, username, password, folder, tls);
    }
    
    // 释放动态分配的内存空间
    free(username);
    free(password);
    free(folder);
    free(command);
    free(server_name);
    free(mesg_num);
    
    return 0;
}
