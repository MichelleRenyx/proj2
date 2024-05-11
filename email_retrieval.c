/*
执行检索和存储邮件
Get mail from the server according to the command.
Process mail data, such as decoding MIME parts.
Print or return email content.
*/
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <openssl/ssl.h>
#include <openssl/err.h>
#include "imap_connection.h"

// 函数声明
void fetch_email(SSL *ssl, const char *tag, const char *command);

// 检索邮件的函数
void fetch_email(SSL *ssl, const char *tag, const char *command) {
    char buffer[1024];
    int len;

    // 发送FETCH命令
    sprintf(buffer, "%s %s\r\n", tag, command);
    if (SSL_write(ssl, buffer, strlen(buffer)) <= 0) {
        fprintf(stderr, "SSL Write Error\n");
        return;
    }

    // 读取响应
    while ((len = SSL_read(ssl, buffer, sizeof(buffer) - 1)) > 0) {
        buffer[len] = '\0';
        printf("%s", buffer);
    }

    if (len < 0) {
        int err = SSL_get_error(ssl, len);
        if (err == SSL_ERROR_WANT_READ || err == SSL_ERROR_WANT_WRITE) {
            printf("SSL read needs to be retried\n");
        } else {
            fprintf(stderr, "SSL Read Error\n");
        }
    }
}

int main() {
    SSL_CTX *ctx = init_ssl_context();
    SSL *ssl = create_ssl_connection("imap.example.com", IMAP_SSL_PORT, ctx);
    if (ssl == NULL) {
        fprintf(stderr, "Failed to establish SSL connection\n");
        return 1;
    }

    fetch_email(ssl, "A01", "FETCH 1 BODY[]");

    // 关闭SSL连接并清理资源
    SSL_shutdown(ssl);
    SSL_free(ssl);

    return 0;
}
