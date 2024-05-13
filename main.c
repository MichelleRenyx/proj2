#include "connection.h"
#include "commands.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int main(int argc, char *argv[]) {
    const char *username = NULL;
    const char *password = NULL;
    const char *folder = "INBOX"; // 默认文件夹为INBOX
    const char *command = NULL;
    const char *server_name = NULL;
    const char *messageNum = NULL;
    int use_tls = 0; // 默认不使用TLS
    int port = use_tls ? IMAP_SSL_PORT : IMAP_PORT;

    int sockfd = create_socket(server_name, port);
    if (use_tls) {
        use_tls = 1;
        sockfd = upgrade_to_tls(sockfd);
    }

    for (int i = 1; i < argc; i++) {
        if (strcmp(argv[i], "-u") == 0 && i + 1 < argc) {
            username = argv[++i];
        } else if (strcmp(argv[i], "-p") == 0 && i + 1 < argc) {
            password = argv[++i];
        } else if (strcmp(argv[i], "-f") == 0 && i + 1 < argc) {
            folder = argv[++i];
        } else if (strcmp(argv[i], "-n") == 0 && i + 1 < argc) {
            messageNum = argv[++i];
        } else if (strcmp(argv[i], "-t") == 0) {
            use_tls = 1;
        } else if (!command && (strcmp(argv[i], "retrieve") == 0 || strcmp(argv[i], "parse") == 0 || strcmp(argv[i], "mime") == 0 || strcmp(argv[i], "list") == 0)) {
            command = argv[i];
        } else if (!server_name) {
            server_name = argv[i];
        }
    }

    if (!username || !password || !command || !server_name) {
        fprintf(stderr, "Usage: %s -u <username> -p <password> [-f <folder>] [-n <messageNum>] [-t] <command> <server_name>\n", argv[0]);
        return 1;
    }

    

    printf("Username: %s\n", username);
    printf("Password: %s\n", password);
    printf("Folder: %s\n", folder);
    printf("Command: %s\n", command);
    printf("Server: %s\n", server_name);
    printf("Message Number: %s\n", messageNum ? messageNum : "Not specified");
    printf("Use TLS: %d\n", use_tls);


    login_imap(sockfd, username, password);

    select_folder(sockfd, folder);
    
    if (strcmp(command, "retrieve") == 0) {
        fetch_email(sockfd, messageNum);
        close(sockfd);
        exit(0);  // 成功检索后退出
    }

    if (strcmp(command, "parse") == 0) {
        parse_email_headers(sockfd, messageNum);
        close(sockfd);
        exit(0);  // 成功解析后退出
    }

    if (strcmp(command, "mime") == 0) {
        decode_mime_message(sockfd, messageNum);
        close(sockfd);
        exit(0);  // 成功解析后退出
    }

    if (strcmp(command, "list") == 0) {
        list_email_subjects(sockfd);
        close(sockfd);
        exit(0);  // 成功列出后退出
    }

    close(sockfd);
    return 0;
}

