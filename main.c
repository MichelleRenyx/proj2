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
    int *tls = 0;

    read_command_line(argc, argv, username, password, folder, command, server_name, mesg_num, &tls);
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

    return 0;
}