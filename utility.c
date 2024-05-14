
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <arpa/inet.h>
#include <stdbool.h>
#include "utility.h"
#include "commands.h"
#include "connection.h"

#define MAX_TAG_NUMBER 100

static int TAG_NUMBER = 10;

// void read_command_line(int argc, char **argv, char *username, char *password, char *folder, char *command, char *server_name, char *mesg_num, int *tls) {


// }

void toUpperCase(char *str) {
    for (int i = 0; str[i] ; i++) {
        str[i] = toupper((unsigned char)str[i]);
    }
}

char* generate_tag() {
    static char result[10];
    sprintf(result, "X%d", TAG_NUMBER);
    TAG_NUMBER++;
    return result;
}

void print_headers(const char *overview) {
    char *lowercase = strdup(overview);
    for (int i = 0; lowercase[i]; i++) {
        lowercase[i] = tolower(lowercase[i]);
    }

    int idx1 = strstr(lowercase, "from: ") - lowercase;
    int idx2 = strstr(lowercase, "to: ") - lowercase;
    int idx3 = strstr(lowercase, "date: ") - lowercase;
    int idx4 = strstr(lowercase, "subject: ") - lowercase;

    int indexes[4] = {idx1, idx2, idx3, idx4};
    for (int i = 0; i < 4; i++) {
        for (int j = i + 1; j < 4; j++) {
            if (indexes[i] > indexes[j]) {
                int temp = indexes[i];
                indexes[i] = indexes[j];
                indexes[j] = temp;
            }
        }
    }

    for (int i = 0; i < 4; i++) {
        if (indexes[i] == idx1) {
            printf("From: %s\n", overview + 5);
        } else if (indexes[i] == idx2) {
            printf("To: %s\n", overview + 3);
        } else if (indexes[i] == idx3) {
            printf("Date: %s\n", overview + 5);
        } else if (indexes[i] == idx4) {
            printf("Subject: %s\n", overview + 8);
        }
    }

    free(lowercase);
}

int get_total_message(const char *folder, int sockfd) {
//    static char total[10];
    int total = 0;
    char select_folder[256];
    sprintf(select_folder, "1 SELECT %s\r\n", folder);
    send(sockfd, select_folder, strlen(select_folder), 0);

    char buffer[1024];
    int ret;bool flag = false;
    while (true) {
        memset(buffer, 0, sizeof(buffer));
        read_line(sockfd, buffer, sizeof(buffer));
//        toUpper(byteList.bytes);
        toUpperCase(buffer);
        printf("%s\n", buffer);
        if(!flag && sscanf(buffer,  "* %d EXISTS\r\n", &total) == 1) {
            ret = total;
            flag = true;
        }

        printf("%d\n", ret);
        if (buffer[0] && buffer[0] != '*') {
            printf("非空非星号%s\n", buffer);
            break;
        }
    }

    if (strncasecmp(buffer, "1 OK", 4) != 0) {
        printf("Folder not found\n");
        exit(3);
    }
    return ret;
}

int valid_message_number(const char *mesg_num) {
    for (int i = 0; i < strlen(mesg_num); i++) {
        if (!isdigit(mesg_num[i])) {
            return 0;
        }
    }
    return 1;
}

int valid_server_name(const char *server_name) {
    if (server_name[0] == '-' || strlen(server_name) >= 255) {
        return 0;
    }
    for (int i = 0; i < strlen(server_name); i++) {
        if (!isalnum(server_name[i]) && server_name[i] != '-' && server_name[i] != '.') {
            return 0;
        }
    }
    return 1;
}

int valid_ip(const char *address) {
    struct sockaddr_in sa;
    return inet_pton(AF_INET, address, &(sa.sin_addr)) != 0;
}

int valid_folder_name(const char *folder) {
    if (strlen(folder) == 0) {
        return 0;
    }
    for (int i = 0; i < strlen(folder); i++) {
        if ((folder[i] & 0x80) != 0 || folder[i] == '&' || folder[i] == '#') {
            return 0;
        }
    }
    return 1;
}

int check_mime_header(const char *part, int mode) {
    char *copy = strdup(part);
    int sum = 0;
    char *lowercase = strdup(copy);
    for (int i = 0; lowercase[i]; i++) {
        lowercase[i] = tolower(lowercase[i]);
    }

    if (strstr(lowercase, "content-type: text/plain")) {
        sum++;
        if (!mode) {
            free(copy);
            free(lowercase);
            return 1;
        }
    }
    if (strstr(lowercase, "charset=utf-8")) {
        sum++;
        if (!mode) {
            free(copy);
            free(lowercase);
            return 1;
        }
    }
    if (strstr(lowercase, "content-transfer-encoding: quoted-printable") || strstr(lowercase, "content-transfer-encoding: 7bit") || strstr(lowercase, "content-transfer-encoding: 8bit")) {
        sum++;
        if (!mode) {
            free(copy);
            free(lowercase);
            return 1;
        }
    }

    free(copy);
    free(lowercase);
    return mode && sum == 3;
}