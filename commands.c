#include "commands.h"
#include "connection.h"
#include <arpa/inet.h>
#include <netinet/in.h>
#include <openssl/ssl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netinet/ip.h> // 对于IPv4和IPv6的通用支持
#include <netdb.h>

// Helper function to send commands to the server
void send_command(int sockfd, const char *command) {
    if (write(sockfd, command, strlen(command)) < 0) {
        perror("Write to socket failed");
        exit(4);
    }
}


// char *receive_response(int sockfd) {
//   char *response = malloc(4096);
//   if (!response) {
//     fprintf(stderr, "Memory allocation failed\n");
//     return NULL;
//   }

//   char buffer[1024];
//   int total_received = 0;
//   while (1) {
//     int nbytes = recv(sockfd, buffer, sizeof(buffer) - 1, 0);
//     if (nbytes < 0) {
//       perror("recv failed");
//       free(response);
//       return NULL;
//     } else if (nbytes == 0) {
//       // No more data, server closed connection
//       break;
//     }

//     buffer[nbytes] = '\0'; // Null-terminate the buffer

//     // Check if the received buffer is enough or needs continuation
//     if (total_received + nbytes < 4096 - 1) {
//       memcpy(response + total_received, buffer, nbytes);
//       total_received += nbytes;
//     } else {
//       // Buffer overflow protection
//       memcpy(response + total_received, buffer, 4096 - 1 - total_received);
//       total_received = 4096 - 1;
//       break;
//     }

  //   // Check for end of command sequence, e.g., "a1 OK"
  //   if (strstr(response, "a1 OK") != NULL ||
  //       strstr(response, "a1 NO") != NULL) {
  //     break;
  //   }
  // }

//   response[total_received] = '\0'; // Ensure response is null-terminated
//   return response;
// }

// void receive_response(int sockfd, char *buffer, size_t size) {
//     memset(buffer, 0, size);
//     ssize_t n = read(sockfd, buffer, size - 1);
//     if (n < 0) {
//         printf("Read from socket failed\n");
//         exit(2);
//     }
//     if (n == 0) {
//         printf("Unexpected disconnection\n");
//         exit(2);
//     }
// }
// Helper function to receive responses from the server
void receive_response(int sockfd, char *buffer, size_t size) {
    memset(buffer, 0, size);
    if (read(sockfd, buffer, size - 1) < 0) {
        perror("Read from socket failed");
        exit(4);
    }
}
// void login_imap(int sockfd, const char *username, const char *password) {
//   char login_command[1024];
//   sprintf(login_command, "a1 LOGIN %s %s\r\n", username, password);
//   send_command(sockfd, login_command);

//   char response[1024];
//   if (read(sockfd, response, sizeof(response)) <= 0) {
//     perror("Failed to read response");
//     close(sockfd);
//     exit(EXIT_FAILURE);
//   }

//   if (strstr(response, "a1 OK") == NULL) {
//     fprintf(stderr, "Login failed: %s", response);
//     close(sockfd);
//     exit(EXIT_FAILURE);
//   }
// }

// void select_folder(int sockfd, const char *folder) {
//   char command[1024];
//   snprintf(command, sizeof(command), "A02 SELECT %s\r\n", folder);
//   send_command(sockfd, command);
//   char *response = receive_response(sockfd);
//   if (strstr(response, "A02 OK") == NULL) {
//     printf("Folder not found\n");
//     free(response);
//     exit(3);
//   }
//   free(response);
// }

// void fetch_email(int sockfd, const char *messageNum) {
//   char command[1024];
//   snprintf(command, sizeof(command),
//            (messageNum && strlen(messageNum) > 0)
//                ? "A03 FETCH %s BODY.PEEK[]\r\n"
//                : "A03 FETCH 1:* BODY.PEEK[]\r\n",
//            messageNum);
//   send_command(sockfd, command);
//   char *response = receive_response(sockfd);
//   if (strstr(response, "A03 OK") == NULL) {
//     fprintf(stderr, "Message not found\n");
//     free(response);
//     exit(3);
//   }
//   printf("%s", response); // 打印邮件内容
//   free(response);
// }
void retrieve(int sockfd, const char *messageNum) {
    char command[1024];
    snprintf(command, sizeof(command), "A07 FETCH %s BODY.PEEK[]\r\n", messageNum);
    send_command(sockfd, command);

    char response[4096];
    receive_response(sockfd, response, sizeof(response));
    if (strstr(response, "NO")) {
        printf("Message not found\n");
        exit(3);
    }
    printf("Email content: %s\n", response);
}
// 解析邮件头部信息
// void parse_email_headers(int sockfd, const char *messageNum) {
//   char command[1024];
//   snprintf(command, sizeof(command),
//            (messageNum && strlen(messageNum) > 0)
//                ? "A04 FETCH %s BODY.PEEK[HEADER]\r\n"
//                : "A04 FETCH 1:* BODY.PEEK[HEADER]\r\n",
//            messageNum);
//   send_command(sockfd, command);
//   char *response = receive_response(sockfd);
//   if (strstr(response, "A04 OK") == NULL) {
//     fprintf(stderr, "Message not found\n");
//     free(response);
//     exit(3);
//   }

//   // 解析并打印邮件头部信息
//   char *line = strtok(response, "\r\n");
//   while (line) {
//     if (strncasecmp(line, "From: ", 6) == 0) {
//       printf("From: %s\n", line + 6);
//     } else if (strncasecmp(line, "To: ", 4) == 0) {
//       printf("To: %s\n", line + 4);
//     } else if (strncasecmp(line, "Date: ", 6) == 0) {
//       printf("Date: %s\n", line + 6);
//     } else if (strncasecmp(line, "Subject: ", 9) == 0) {
//       printf("Subject: %s\n", line + 9);
//     }
//     line = strtok(NULL, "\r\n");
//   }
//   free(response);
// }
// Function to parse email headers
void parse(int sockfd, const char *messageNum) {
    char command[1024];
    snprintf(command, sizeof(command), "A08 FETCH %s BODY.PEEK[HEADER]\r\n", messageNum ? messageNum : "1:*");
    send_command(sockfd, command);

    char response[4096];
    receive_response(sockfd, response, sizeof(response));
    if (strstr(response, "NO")) {
        printf("Message not found\n");
        exit(3);
    }
    printf("Email headers: %s\n", response);
}
// // 解码 MIME 消息
// void decode_mime_message(int sockfd, const char *messageNum) {
//   char command[1024];
//   snprintf(command, sizeof(command),
//            (messageNum != NULL && strlen(messageNum) > 0)
//                ? "A05 FETCH %s BODY.PEEK[]\r\n"
//                : "A05 FETCH 1:* BODY.PEEK[]\r\n",
//            messageNum);
//   send_command(sockfd, command);
//   char *response = receive_response(sockfd);
//   if (strstr(response, "A05 OK") == NULL) {
//     fprintf(stderr, "Message not found\n");
//     free(response);
//     exit(4);
//   }

//   // 解析 MIME 消息
//   const char *boundary_start = strstr(response, "boundary=\"");
//   if (!boundary_start) {
//     fprintf(stderr, "MIME boundary not found\n");
//     free(response);
//     exit(4);
//   }
//   boundary_start += 10; // 跳过 "boundary=\""
//   const char *boundary_end = strchr(boundary_start, '\"');
//   if (!boundary_end) {
//     fprintf(stderr, "MIME boundary not properly terminated\n");
//     free(response);
//     exit(4);
//   }
//   size_t boundary_len = boundary_end - boundary_start;
//   char boundary[1024];
//   strncpy(boundary, boundary_start, boundary_len);
//   boundary[boundary_len] = '\0';

//   // 查找并打印第一个 text/plain 部分
//   char *part = strstr(response, boundary);
//   while (part) {
//     part = strstr(part + 1, boundary);
//     if (part && strstr(part, "Content-Type: text/plain")) {
//       char *content_start = strstr(part, "\r\n\r\n");
//       if (content_start) {
//         content_start += 4; // 跳过空行到内容开始
//         char *content_end = strstr(content_start, boundary);
//         if (content_end) {
//           printf("%.*s\n", (int)(content_end - content_start), content_start);
//           break;
//         }
//       }
//     }
//   }

//   free(response);
// }

// Function to decode MIME messages
void mime(int sockfd, const char *messageNum) {
    char command[1024];
    snprintf(command, sizeof(command), "A09 FETCH %s BODY.PEEK[MIME]\r\n", messageNum ? messageNum : "1:*");
    send_command(sockfd, command);

    char response[4096];
    receive_response(sockfd, response, sizeof(response));
    if (!strstr(response, "text/plain")) {
        printf("Match failed\n");
        exit(4);
    }
    printf("MIME content: %s\n", response);
}

// // 列出邮件主题
// void list_email_subjects(int sockfd) {
//   char command[1024];
//   snprintf(command, sizeof(command),
//            "A06 FETCH 1:* (BODY.PEEK[HEADER.FIELDS (SUBJECT)])\r\n");
//   send_command(sockfd, command);
//   char *response = receive_response(sockfd);
//   if (strstr(response, "A06 OK") == NULL) {
//     fprintf(stderr, "Error retrieving subjects\n");
//     free(response);
//     exit(4);
//   }

//   // 解析并打印主题
//   char *line = strtok(response, "\r\n");
//   int msg_num = 1;
//   while (line != NULL) {
//     if (strstr(line, "SUBJECT:")) {
//       char *subject_start = line + 8; // 跳过 "SUBJECT:" 字符串
//       while (*subject_start == ' ')
//         subject_start++; // 跳过额外的空格
//       char *subject_end = subject_start + strlen(subject_start) - 1;
//       while (*subject_end == ' ' || *subject_end == '\r' ||
//              *subject_end == '\n')
//         subject_end--; // 去除尾部空格
//       *(subject_end + 1) = '\0';
//       printf("%d: %s\n", msg_num++, subject_start);
//     }
//     line = strtok(NULL, "\r\n");
//   }

//   if (msg_num == 1) {
//     // 没有找到任何主题，可能邮箱为空
//     exit(0);
//   }
// }

// Function to list email subjects
void list(int sockfd) {
    char command[] = "A10 FETCH 1:* BODY.PEEK[HEADER.FIELDS (SUBJECT)]\r\n";
    send_command(sockfd, command);

    char response[4096];
    receive_response(sockfd, response, sizeof(response));
    if (strlen(response) == 0) {
        exit(0);  // 邮箱为空
    }
    printf("List of subjects: %s\n", response);
}

// int create_socket(const char *hostname, int port) {
//   int sockfd = socket(AF_INET, SOCK_STREAM, 0);
//   if (sockfd < 0) {
//     perror("Cannot create socket");
//     exit(2);
//   }

//   struct sockaddr_in server_addr;
//   memset(&server_addr, 0, sizeof(server_addr));
//   server_addr.sin_family = AF_INET;
//   server_addr.sin_port = htons(port);

//   if (inet_pton(AF_INET, hostname, &server_addr.sin_addr) <= 0) {
//     perror("Invalid address");
//     close(sockfd);
//     exit(2);
//   }

//   if (connect(sockfd, (struct sockaddr *)&server_addr, sizeof(server_addr)) <
//       0) {
//     perror("Connection Failed");
//     close(sockfd);
//     exit(2);
//   }

//   return sockfd; // 返回 socket 文件描述符
// }

/*IPv4或IPv6*/
// int create_socket(const char *hostname, int port) {
//     int sockfd = -1;
//     struct addrinfo hints, *res, *result;
    

//     // 初始化地址信息结构体
//     memset(&hints, 0, sizeof(hints));
//     hints.ai_family = AF_UNSPEC; // IPv4 或 IPv6
//     hints.ai_socktype = SOCK_STREAM; // TCP

//     // 转换端口号为字符串
//     char port_str[6];
//     snprintf(port_str, sizeof(port_str), "%d", port);

//     // 解析地址信息
//     if (getaddrinfo(hostname, port_str, &hints, &result) != 0) {
//         perror("getaddrinfo failed");
//         return -1;
//     }

//     // 尝试连接服务器
//     for (res = result; res != NULL; res = res->ai_next) {
//         sockfd = socket(res->ai_family, res->ai_socktype, res->ai_protocol);
//         if (sockfd == -1) {
//             perror("socket failed");
//             continue;
//         }

//         if (connect(sockfd, res->ai_addr, res->ai_addrlen) == 0)
//             break; // 成功连接

//         close(sockfd);
//     }

//     // 清理
//     freeaddrinfo(result);
//     if (sockfd == -1) { // 检查是否成功连接
//         fprintf(stderr, "Could not connect to %s on port %d\n", hostname, port);
//     }

//     return sockfd;
// }

int create_socket(const char *hostname, int port) {
    int sockfd;
    struct sockaddr_in serv_addr;

    // 创建 socket
    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd < 0) {
        perror("Error creating socket");
        exit(2);
    }

    // 清零 serv_addr
    memset(&serv_addr, 0, sizeof(serv_addr));
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(port);

    // 将主机名转换为 IP 地址
    if (inet_pton(AF_INET, hostname, &serv_addr.sin_addr) <= 0) {
        printf("Invalid address/Address not supported\n");
        exit(2);
    }

    // 连接到服务器
    if (connect(sockfd, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0) {
        printf("Connection Failed\n");
        exit(2);
    }

    return sockfd;