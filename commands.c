#include "commands.h"
#include "connection.h"
#include "utility.h"
#include <arpa/inet.h>
#include <netinet/in.h>
#include <openssl/ssl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <unistd.h>
#include <ctype.h>
void retrieve(const char *server_name, const char *username, const char *password, const char *folder, const char *mesg_num, int tls) {
    int sockfd = connect_login(server_name, username, password, tls);
    char *total = get_total_message(folder, sockfd);

    char fetch_command[256];
    char tag[10];
    strcpy(tag, generate_tag());
    if (strlen(mesg_num) > 0) {
        sprintf(fetch_command, "%s FETCH %s BODY.PEEK[]\r\n", tag, mesg_num);
    } else {
        sprintf(fetch_command, "%s FETCH %s BODY.PEEK[]\r\n", tag, total);
    }
    send(sockfd, fetch_command, strlen(fetch_command), 0);

    char buffer[1024];
    char response[8192] = {0};
    int start = 0;

    while (recv(sockfd, buffer, sizeof(buffer), 0) > 0) {
        if (strstr(buffer, "BAD Error in IMAP command FETCH") != NULL) {
            fprintf(stderr, "Message not found\n");
            exit(3);
        }
        if (strstr(buffer, "FETCH") != NULL) {
            start = 1;
        }
        if (strstr(buffer, "fetch completed") != NULL) {
            break;
        }
        if (start && strstr(buffer, "FETCH") == NULL) {
            // strcat(response, buffer);
            strncat(response, buffer, sizeof(response) - strlen(response) - 1);
        }
    }

    printf("%s", response);
    safe_disconnect(sockfd);
}

void parse(const char *server_name, const char *username, const char *password, const char *folder, const char *mesg_num, int tls) {
    int sockfd = connect_login(server_name, username, password, tls);
    char *total = get_total_message(folder, sockfd);

    char fetch_command[256];
    char tag[10];
    strcpy(tag, generate_tag());
    if (strlen(mesg_num) > 0) {
        sprintf(fetch_command, "%s FETCH %s BODY.PEEK[HEADER.FIELDS (FROM TO DATE SUBJECT)]\r\n", tag, mesg_num);
    } else {
        sprintf(fetch_command, "%s FETCH %s BODY.PEEK[HEADER.FIELDS (FROM TO DATE SUBJECT)]\r\n", tag, total);
    }
    send(sockfd, fetch_command, strlen(fetch_command), 0);

    char buffer[1024];
    char response[8192] = {0};
    int start = 0;

    while (recv(sockfd, buffer, sizeof(buffer), 0) > 0) {
        if (strstr(buffer, "BAD Error in IMAP command FETCH") != NULL) {
            fprintf(stderr, "Message not found\n");
            exit(3);
        }
        if (strstr(buffer, "FETCH (BODY[HEADER.FIELDS (FROM TO DATE SUBJECT)]") != NULL) {
            start = 1;
            continue;
        }
        if (strstr(buffer, "Fetch completed") != NULL) {
            break;
        }
        if (start) {
            // strcat(response, buffer);
            strncat(response, buffer, sizeof(response) - strlen(response) - 1);
        }
    }

    print_headers(response);
    safe_disconnect(sockfd);
}

void mime(const char *server_name, const char *username, const char *password, const char *folder, const char *mesg_num, int tls) {
    char response[8192];
    retrieve(server_name, username, password, folder, mesg_num, tls);

    char *mime_position = strstr(response, "mime-version: 1.0");
    char *media_type_position = strstr(response, "multipart/alternative");

    if (mime_position == NULL || media_type_position == NULL) {
        fprintf(stderr, "Match failed\n");
        exit(4);
    } else {
        char boundary[256];
        strncpy(boundary, strstr(response, "boundary=") + 10, sizeof(boundary) - 1);
        strtok(boundary, "\";");

        char *first_match = NULL;
        char *split_by_bound = strtok(response, boundary);

        while (split_by_bound != NULL) {
            if (check_mime_header(split_by_bound, 1)) {
                first_match = split_by_bound;
                break;
            }
            split_by_bound = strtok(NULL, boundary);
        }

        if (first_match == NULL) {
            fprintf(stderr, "Match failed\n");
            exit(4);
        }

        first_match += 2;

        char result[8192] = {0};
        char *line = strtok(first_match, "\r\n");
        while (line != NULL) {
            if (check_mime_header(line, 0)) {
                line = strtok(NULL, "\r\n");
                continue;
            }
            if (strstr(line, boundary) != NULL || strstr(line, boundary) != NULL) {
                break;
            }
            // strcat(result, line);
            // strcat(result, "\r\n");
            strncat(result, line, sizeof(result) - strlen(result) - 1);
            strncat(result, "\r\n", sizeof(result) - strlen(result) - 1);
            line = strtok(NULL, "\r\n");
        }

        printf("%s", result);
    }
}

void list(const char *server_name, const char *username, const char *password, const char *folder, int tls) {
    int sockfd = connect_login(server_name, username, password, tls);
    char list[8192] = {0};
    char *total = get_total_message(folder, sockfd);

    int total_num = atoi(total);
    for (int i = 1; i <= total_num; i++) {
        char fetch_command[256];
        char tag[10];
        strcpy(tag, generate_tag());
        //sprintf(fetch_command, "%s FETCH %d BODY.PEEK[HEADER.FIELDS (SUBJECT)]\r\n", tag, i);
        snprintf(fetch_command, sizeof(fetch_command), "%s FETCH %d BODY.PEEK[HEADER.FIELDS (SUBJECT)]\r\n", tag, i);
        send(sockfd, fetch_command, strlen(fetch_command), 0);

        char buffer[1024];
        while (recv(sockfd, buffer, sizeof(buffer), 0) > 0) {
            // strcat(list, buffer);
            strncat(list, buffer, sizeof(list) - strlen(list) - 1);
            if (strstr(buffer, "ok fetch completed") != NULL) {
                break;
            }
        }
    }

    char *lowercase = strdup(list);
    for (int i = 0; lowercase[i]; i++) {
        lowercase[i] = tolower(lowercase[i]);
    }

    char *splited[256];
    int idx = 0;
    char *line = strtok(list, "\r\n");
    while (line != NULL) {
        splited[idx++] = line;
        line = strtok(NULL, "\r\n");
    }

    int mesg_num = 1;
    for (int i = 0; i < idx; i++) {
        if (strstr(lowercase, "* fetch")) {
            if (strstr(splited[i], "subject:") != NULL) {
                printf("%d: %s\n", mesg_num, splited[i] + 9);
                mesg_num++;
            } else {
                printf("%d: <No subject>\n", mesg_num);
                mesg_num++;
            }
        }
    }

    free(lowercase);
    safe_disconnect(sockfd);
}
