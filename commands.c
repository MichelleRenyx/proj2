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
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <errno.h>
#include "utility.h"
#include "connection.h"
static int tagNum = 0;


ssize_t read_line(int sockfd, char *buffer, size_t max_len) {
    ssize_t n, rc;
    char c, *ptr;

    ptr = buffer;
    for (n = 1; n < max_len; n++) {
        if ((rc = recv(sockfd, &c, 1, 0)) == 1) {
            *ptr++ = c;
            if (c == '\n') {
                break;
            }
        } else if (rc == 0) {
            if (n == 1) {
                return 0;
            } else {
                break;
            }
        } 
    }

    *ptr = 0;
    
    return n;
}

// 自定义逐字节读取函数
ssize_t read_bytes(int sockfd, char *buffer, size_t num_bytes) {
    ssize_t n;
    size_t total_bytes = 0;
    while (total_bytes < num_bytes) {
        n = recv(sockfd, buffer + total_bytes, num_bytes - total_bytes, 0);
        if (n <= 0) {
            return -1;
        }
        total_bytes += n;
    }
    buffer[total_bytes] = '\0';
    return total_bytes;
}

void retrieve(const char *server_name, const char *username, const char *password, const char *folder, const char *mesg_num, int tls) {
    int sockfd = connect_login(server_name, tls);
    int total = get_total_message(folder, sockfd);
    // printf("total = %d\n", total);

    char fetch_command[1024];
//    char tag[10];
//    snprintf(tag, sizeof(tag), "A%d", ++tagNum);
//    printf("tag %s\n", tag);
    // printf("mesg_num %s\n", mesg_num);
    if (mesg_num) {
        sprintf(fetch_command, "A%d FETCH %s BODY.PEEK[]\r\n", ++tagNum, mesg_num);
//        printf("strlen(mesg_num) > 0 fetch_command %s\n", fetch_command);
        // fflush(stdout);printf("妹");
        // fflush(stdout);
    } else {
        sprintf(fetch_command, "A%d FETCH %d BODY.PEEK[]\r\n", ++tagNum, total);
//        printf("strlen(mesg_num) bu> 0 fetch_command %s\n", fetch_command);
    }
    // printf("妹");printf("妹");printf("妹");
    // fflush(stdout);
    send(sockfd, fetch_command, strlen(fetch_command), 0);

    // printf("send了");
    // fflush(stdout);

    char buffer[65535];
    int messageNum, bodyL;
//    memset(buffer, 0, sizeof(buffer));
    read_line(sockfd, buffer, sizeof(buffer));
    if (sscanf(buffer, "* %d FETCH (BODY[] {%d}\r\n", &messageNum, &bodyL) != 2) {
        // printf("妹有FETCH (BODY[] {}  %s", buffer);
        printf("Message not found\n");
        exit(3);
    }
    // printf("bodyL = %d", bodyL);
    // fflush(stdout);

    read_bytes(sockfd, buffer, bodyL);
    // fflush(stdout);
    // printf("又来全 %s", buffer);
    // fflush(stdout);
    read_line(sockfd, buffer, sizeof(buffer));
    // printf("又来1 %s", buffer);
    // fflush(stdout);
    read_line(sockfd, buffer, sizeof(buffer));
    // printf("又来2 %s", buffer);
    // fflush(stdout);

    snprintf(fetch_command, sizeof(fetch_command), "A%d OK ", messageNum);
    if (strncasecmp(buffer, fetch_command, strlen(fetch_command)) != 0) {
        printf("Message not found\n");
        exit(3);
    }

    safe_disconnect(sockfd);
}
// 自定义 strcasestr 实现，如果系统不支持 strcasestr，可以使用这个实现
// char *strcasestr(const char *haystack, const char *needle) {
//     if (!*needle) return (char *)haystack;
//     for ( ; *haystack; ++haystack) {
//         if (toupper((unsigned char)*haystack) == toupper((unsigned char)*needle)) {
//             const char *h, *n;
//             for (h = haystack, n = needle; *h && *n; ++h, ++n) {
//                 if (toupper((unsigned char)*h) != toupper((unsigned char)*n)) {
//                     break;
//                 }
//             }
//             if (!*n) return (char *)haystack;
//         }
//     }
//     return NULL;
// }

// void retrieve(const char *server_name, const char *username, const char *password, const char *folder, const char *mesg_num, int tls) {
//     int sockfd = connect_login(server_name, username, password, tls);
//     char *total = get_total_message(folder, sockfd);

//     char fetch_command[256];
//     char tag[10];
//     strcpy(tag, generate_tag());
//     if (strlen(mesg_num) > 0) {
//         snprintf(fetch_command, sizeof(fetch_command), "%s FETCH %s BODY.PEEK[]\r\n", tag, mesg_num);
//     } else {
//         snprintf(fetch_command, sizeof(fetch_command), "%s FETCH %s BODY.PEEK[]\r\n", tag, total);
//     }
//     send(sockfd, fetch_command, strlen(fetch_command), 0);

//     char buffer[65535];
//     char response[12077] = {0};
//     int start = 0;
//     int end_reached = 0;
//     // printf("进入while循环了\n");

//     while (1) {
//         int bytes_received = recv(sockfd, buffer, sizeof(buffer) - 1, 0);
//         if (bytes_received < 0) {
//             fprintf(stderr, "Error receiving data\n");
//             exit(3);
//         } else if (bytes_received == 0) {
//             // No more data from server
//             break;
//         }

//         buffer[bytes_received] = '\0'; // Ensure null-terminated string

//         // Debugging output
//         // printf("Received buffer: %s\n", buffer);

//         if (strcasestr(buffer, "BAD Error in IMAP command FETCH") != NULL) {
//             fprintf(stderr, "Message not found\n");
//             exit(3);
//         }

//         if (strcasestr(buffer, "FETCH") != NULL) {
//             start = 1;
//             //Debug printf("Detected FETCH\n");
//         }

//         if (strcasestr(buffer, "fetch completed") != NULL) {
//             end_reached = 1;
//             //Debug printf("Detected fetch completed\n");
//         }

//         if (start && strcasestr(buffer, "FETCH") == NULL) {
//             strncat(response, buffer, sizeof(response) - strlen(response) - 1);
//         }

//         if (end_reached) {
//             break;
//         }
//     }

//     // printf("退出while循环了\n");
//     printf("%s", response); 
//     // printf("Received buffer: %s\n", buffer);
//     safe_disconnect(sockfd);
// }

void parse(const char *server_name, const char *username, const char *password, const char *folder, const char *mesg_num, int tls) {
    int sockfd = connect_login(server_name, tls);
    int total = get_total_message(folder, sockfd);

    char fetch_command[256];
    char tag[10];
    strcpy(tag, generate_tag());
    if (strlen(mesg_num) > 0) {
        sprintf(fetch_command, "%s FETCH %s BODY.PEEK[HEADER.FIELDS (FROM TO DATE SUBJECT)]\r\n", tag, mesg_num);
    } else {
        sprintf(fetch_command, "%s FETCH %d BODY.PEEK[HEADER.FIELDS (FROM TO DATE SUBJECT)]\r\n", tag, total);
    }
    //printf("fetch_command: %s   TO sent\n", fetch_command);
    send(sockfd, fetch_command, strlen(fetch_command), 0);
    // printf("fetch_command: %s   HAS been sent\n", fetch_command);

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
    int sockfd = connect_login(server_name, tls);
    char list[8192] = {0};
    int total = get_total_message(folder, sockfd);

    int total_num = total;
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
