#include "commands.h"
#include "connection.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <openssl/ssl.h>

int main(int argc, char *argv[]) {
    const char *username = NULL;
    const char *password = NULL;
    const char *folder = "INBOX"; // 默认文件夹为INBOX
    const char *command = NULL;
    const char *server_name = NULL;
    const char *messageNum = NULL;
    int use_tls = 0, port; // 默认不使用TLS

    // 解析命令行参数
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
            use_tls = 1; // 启用TLS
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

    int port = use_tls ? IMAP_SSL_PORT : IMAP_PORT;
    SSL *ssl = create_socket(server_name, port, use_tls);

    if (!ssl) {
        fprintf(stderr, "Failed to establish SSL connection.\n");
        return 2;
    }


    login_imap(ssl, username, password);
    select_folder(ssl, folder);
    
    if (strcmp(command, "retrieve") == 0) {
        fetch_email(ssl, messageNum);
    } else if (strcmp(command, "parse") == 0) {
        parse_email_headers(ssl, messageNum);
    } else if (strcmp(command, "mime") == 0) {
        decode_mime_message(ssl, messageNum);
    } else if (strcmp(command, "list") == 0) {
        list_email_subjects(ssl);
    }

    // 清理
    SSL_shutdown(ssl);
    SSL_free(ssl);
    exit(0); // 成功执行后退出
}


