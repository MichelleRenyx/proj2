#include "commands.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <openssl/ssl.h>


void send_command(SSL *ssl, const char *command) {
    if (SSL_write(ssl, command, strlen(command)) <= 0) {
        ERR_print_errors_fp(stderr);
        exit(EXIT_FAILURE);
    }
}

char* receive_response(SSL *ssl) {
    char *response = malloc(4096);
    if (!response) {
        fprintf(stderr, "Memory allocation failed\n");
        exit(EXIT_FAILURE);
    }

    int nbytes = SSL_read(ssl, response, 4096 - 1);
    if (nbytes < 0) {
        ERR_print_errors_fp(stderr);
        free(response);
        exit(EXIT_FAILURE);
    }

    response[nbytes] = '\0';
    return response;
}

void login_imap(SSL *ssl, const char *username, const char *password) {
    char command[1024];
    snprintf(command, sizeof(command), "A01 LOGIN %s %s\r\n", username, password);
    send_command(ssl, command);
    char *response = receive_response(ssl);
    if (strstr(response, "A01 OK") == NULL) {
        printf("Login failure\n");
        free(response);
        exit(3);
    }
    free(response);
}

void select_folder(SSL *ssl, const char *folder) {
    char command[1024];
    snprintf(command, sizeof(command), "A02 SELECT %s\r\n", folder);
    send_command(ssl, command);
    char *response = receive_response(ssl);
    if (strstr(response, "A02 OK") == NULL) {
        printf("Folder not found\n");
        free(response);
        exit(3);
    }
    free(response);
}

char* fetch_email(SSL *ssl, const char *messageNum) {
    char command[1024];
    snprintf(command, sizeof(command), (messageNum && strlen(messageNum) > 0) ? 
        "A03 FETCH %s BODY.PEEK[]\r\n" : "A03 FETCH 1:* BODY.PEEK[]\r\n", messageNum);
    send_command(ssl, command);
    char *response = receive_response(ssl);
    if (strstr(response, "A03 OK") == NULL) {
        printf("Message not found\n");
        free(response);
        exit(3);
    }
    printf("%s", response);  // 打印邮件内容
    free(response);
}


// 解析邮件头部信息
void parse_email_headers(SSL *ssl, const char *messageNum) {
    char command[1024];
    snprintf(command, sizeof(command), (messageNum && strlen(messageNum) > 0) ? 
        "A04 FETCH %s BODY.PEEK[HEADER]\r\n" : "A04 FETCH 1:* BODY.PEEK[HEADER]\r\n", messageNum);
    send_command(ssl, command);
    char *response = receive_response(ssl);
    if (strstr(response, "A04 OK") == NULL) {
        printf("Message not found\n");
        free(response);
        exit(3);
    }

    // 解析并打印邮件头部信息
    char *line = strtok(response, "\r\n");
    while (line) {
        if (strncasecmp(line, "From: ", 6) == 0) {
            printf("From: %s\n", line + 6);
        } else if (strncasecmp(line, "To: ", 4) == 0) {
            printf("To: %s\n", line + 4);
        } else if (strncasecmp(line, "Date: ", 6) == 0) {
            printf("Date: %s\n", line + 6);
        } else if (strncasecmp(line, "Subject: ", 9) == 0) {
            printf("Subject: %s\n", line + 9);
        }
        line = strtok(NULL, "\r\n");
    }
    free(response);
}


// 解码 MIME 消息
void decode_mime_message(SSL *ssl, const char *messageNum) {
    char command[1024];
    if (messageNum != NULL && strlen(messageNum) > 0) {
        snprintf(command, sizeof(command), "A05 FETCH %s BODY.PEEK[]\r\n", messageNum);
    } else {
        snprintf(command, sizeof(command), "A05 FETCH 1:* BODY.PEEK[]\r\n");  // 获取最后一封邮件
    }
    send_command(ssl, command);
    char *response = receive_response(ssl);
    if (strstr(response, "A05 OK") == NULL) {
        printf("Message not found\n");
        free(response);
        exit(4);
    }

    // 解析 MIME 消息
    const char *boundary_start = strstr(response, "boundary=\"");
    if (!boundary_start) {
        printf("MIME boundary not found\n");
        free(response);
        exit(4);
    }
    boundary_start += 10;  // 跳过 "boundary=\""
    const char *boundary_end = strchr(boundary_start, '\"');
    if (!boundary_end) {
        printf("MIME boundary not properly terminated\n");
        free(response);
        exit(4);
    }
    size_t boundary_len = boundary_end - boundary_start;
    char boundary[1024];
    strncpy(boundary, boundary_start, boundary_len);
    boundary[boundary_len] = '\0';

    // 查找并打印第一个 text/plain 部分
    char *part = strstr(response, boundary);
    while (part) {
        part = strstr(part + 1, boundary);
        if (part && strstr(part, "Content-Type: text/plain")) {
            char *content_start = strstr(part, "\r\n\r\n");
            if (content_start) {
                content_start += 4;  // 跳过空行到内容开始
                char *content_end = strstr(content_start, boundary);
                if (content_end) {
                    printf("%.*s\n", (int)(content_end - content_start), content_start);
                    break;
                }
            }
        }
    }

    free(response);
}


// 列出邮件主题
void list_email_subjects(SSL *ssl) {
    char command[1024];
    snprintf(command, sizeof(command), "A06 FETCH 1:* (BODY.PEEK[HEADER.FIELDS (SUBJECT)])\r\n");
    send_command(ssl, command);
    char *response = receive_response(ssl);
    if (strstr(response, "A06 OK") == NULL) {
        printf("Error retrieving subjects\n");
        free(response);
        exit(4);
    }

    // 解析并打印主题
    char *line = strtok(response, "\r\n");
    int msg_num = 1;
    while (line != NULL) {
        if (strstr(line, "SUBJECT:")) {
            char *subject_start = line + 8;  // 跳过 "SUBJECT:" 字符串
            while (*subject_start == ' ') subject_start++;  // 跳过额外的空格
            char *subject_end = subject_start + strlen(subject_start) - 1;
            while (*subject_end == ' ' || *subject_end == '\r' || *subject_end == '\n') subject_end--;  // 去除尾部空格
            *(subject_end + 1) = '\0';
            printf("%d: %s\n", msg_num++, subject_start);
        }
        line = strtok(NULL, "\r\n");
    }

    if (msg_num == 1) {
        // 没有找到任何主题，可能邮箱为空
        exit(0);
    }
}

SSL* create_socket(const char *hostname, int port, int use_tls) {
    int sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd < 0) {
        perror("Cannot create socket");
        exit(2);
    }

    struct sockaddr_in server_addr;
    memset(&server_addr, 0, sizeof(server_addr));
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(port);

    if (inet_pton(AF_INET, hostname, &server_addr.sin_addr) <= 0) {
        perror("Invalid address");
        close(sockfd);
        exit(2);
    }

    if (connect(sockfd, (struct sockaddr *)&server_addr, sizeof(server_addr)) < 0) {
        perror("Connection Failed");
        close(sockfd);
        exit(2);
    }

    if (use_tls) {
        SSL_CTX *ctx = init_ssl_context();
        if (!ctx) {
            close(sockfd);
            exit(2);
        }

        SSL *ssl = SSL_new(ctx);
        if (!ssl) {
            SSL_CTX_free(ctx);
            close(sockfd);
            exit(2);
        }

        SSL_set_fd(ssl, sockfd);
        if (SSL_connect(ssl) != 1) {
            ERR_print_errors_fp(stderr);
            SSL_free(ssl);
            SSL_CTX_free(ctx);
            close(sockfd);
            exit(2);
        }

        // Ensure the server certificate can be verified
        if (SSL_get_verify_result(ssl) != X509_V_OK) {
            fprintf(stderr, "Certificate verification error\n");
            SSL_free(ssl);
            SSL_CTX_free(ctx);
            close(sockfd);
            exit(2);
        }

        return ssl;
    } else {
        // 非 TLS 连接的处理
        return NULL;  // 对于非TLS模式，我们返回NULL表示未使用加密
    }
}

SSL_CTX* init_ssl_context(void) {
    SSL_library_init();
    OpenSSL_add_all_algorithms();
    SSL_load_error_strings();
    const SSL_METHOD *method = TLS_client_method();
    SSL_CTX *ctx = SSL_CTX_new(method);
    if (!ctx) {
        ERR_print_errors_fp(stderr);
        exit(EXIT_FAILURE);
    }

    // 加载信任的证书路径
    if (!SSL_CTX_load_verify_locations(ctx, "/path/to/your/cacert.pem", NULL)) {
        fprintf(stderr, "Failed to load trust certificate\n");
        SSL_CTX_free(ctx);
        exit(EXIT_FAILURE);
    }

    SSL_CTX_set_verify(ctx, SSL_VERIFY_PEER, NULL);  // 启用证书验证

    return ctx;
}

