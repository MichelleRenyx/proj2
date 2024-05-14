#include "utility.h"
#include "commands.h"
#include "connection.h"
#include <openssl/ssl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int main(int argc, char **argv) {
    char username[256] = {0};
    char password[256] = {0};
    char folder[256] = {0};
    char command[256] = {0};
    char server_name[256] = {0};
    char mesg_num[256] = {0};
    int tls = 0;

    read_command_line(argc, argv, username, password, folder, command, server_name, mesg_num, &tls);

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

    return 0;
}