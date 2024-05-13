#include "connection.h"
#include "commands.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
void safe_disconnect(int sockfd) {
    const char *logout_command = "1 LOGOUT\r\n";
    send(sockfd, logout_command, strlen(logout_command), 0);  // Send logout command
    recv(sockfd, NULL, 0, 0);  // Optionally wait for server response, ignored here for simplicity
    close(sockfd);  // Close the socket
}
