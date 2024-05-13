#include "commands.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

void send_command(int sockfd, const char *command) {
    int len = strlen(command);
    if (write(sockfd, command, len) != len) {
        perror("Failed to write to socket");
        exit(EXIT_FAILURE);
    }
}

char* receive_response(int sockfd) {
    char *response = malloc(4096);
    if (!response) {
        fprintf(stderr, "Memory allocation failed\n");
        exit(EXIT_FAILURE);
    }

    int nbytes = read(sockfd, response, 4096 - 1);
    if (nbytes < 0) {
        perror("Failed to read from socket");
        free(response);
        exit(EXIT_FAILURE);
    }

    response[nbytes] = '\0';
    return response;
}

void login_imap(int sockfd, const char *username, const char *password) {
    char command[1024];
    snprintf(command, sizeof(command), "A01 LOGIN %s %s\r\n", username, password);
    send_command(sockfd, command);
    char *response = receive_response(sockfd);
    if (strstr(response, "A01 OK") == NULL) {
        printf("Login failure\n");
        free(response);
        exit(3);
    }
    free(response);
}

void select_folder(int sockfd, const char *folder) {
    char command[1024];
    snprintf(command, sizeof(command), "A02 SELECT %s\r\n", folder);
    send_command(sockfd, command);
    char *response = receive_response(sockfd);
    if (strstr(response, "A02 OK") == NULL) {
        printf("Folder not found\n");
        free(response);
        exit(3);
    }
    free(response);
}

void fetch_email(int sockfd, const char *messageNum) {
    char command[1024];
    if (messageNum != NULL && strlen(messageNum) > 0) {
        snprintf(command, sizeof(command), "A03 FETCH %s BODY.PEEK[]\r\n", messageNum);
    } else {
        snprintf(command, sizeof(command), "A03 FETCH 1:* BODY.PEEK[]\r\n");  // 假设这样来获取最后一封邮件
    }
    send_command(sockfd, command);
    char *response = receive_response(sockfd);
    if (strstr(response, "A03 OK") == NULL) {
        printf("Message not found\n");
        free(response);
        exit(3);
    }
    printf("%s", response);  // 输出邮件内容
    free(response);
}


void parse_email_headers(int sockfd, const char *messageNum) {
    char command[1024];
    if (messageNum != NULL && strlen(messageNum) > 0) {
        snprintf(command, sizeof(command), "A04 FETCH %s BODY.PEEK[HEADER]\r\n", messageNum);
    } else {
        snprintf(command, sizeof(command), "A04 FETCH 1:* BODY.PEEK[HEADER]\r\n");  // 假设这样来获取最后一封邮件的头部
    }
    send_command(sockfd, command);
    char *response = receive_response(sockfd);
    if (strstr(response, "A04 OK") == NULL) {
        printf("Message not found\n");
        free(response);
        exit(3);
    }

    // 处理和打印邮件头部信息
    char *line = strtok(response, "\r\n");
    while (line != NULL) {
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


void decode_mime_message(int sockfd, const char *messageNum) {
    char command[1024];
    if (messageNum != NULL && strlen(messageNum) > 0) {
        snprintf(command, sizeof(command), "A05 FETCH %s BODY.PEEK[]\r\n", messageNum);
    } else {
        snprintf(command, sizeof(command), "A05 FETCH 1:* BODY.PEEK[]\r\n");  // 假设这样来获取最后一封邮件
    }
    send_command(sockfd, command);
    char *response = receive_response(sockfd);
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


void list_email_subjects(int sockfd) {
    char command[1024];
    snprintf(command, sizeof(command), "A06 FETCH 1:* (BODY.PEEK[HEADER.FIELDS (SUBJECT)])\r\n");
    send_command(sockfd, command);
    char *response = receive_response(sockfd);
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

    free(response);
}