/*
Extract "From, To, Date, Subject" and other header information from the original email data.
Process and print output in a standard format.
*/

#include <stdio.h>
#include <string.h>
#include <stdlib.h>

// 解析邮件头部信息
void parse_email_headers(const char *email_data) {
    const char *ptr = email_data;
    char line[1024];

    while (*ptr) {
        const char *next = strchr(ptr, '\n');
        if (!next) break;

        int len = next - ptr;
        if (len > 1023) len = 1023;
        strncpy(line, ptr, len);
        line[len] = '\0';

        // 简单检测和打印头部信息
        if (strstr(line, "From:")) {
            printf("From: %s\n", line + 5);
        } else if (strstr(line, "To:")) {
            printf("To: %s\n", line + 3);
        } else if (strstr(line, "Date:")) {
            printf("Date: %s\n", line + 5);
        } else if (strstr(line, "Subject:")) {
            printf("Subject: %s\n", line + 8);
        }

        ptr = next + 1;
    }
}

int main() {
    // 假设email_data是从服务器获取的原始邮件数据
    const char *email_data = "From: sender@example.com\nTo: receiver@example.com\nDate: Thu, 21 Dec 2000 16:01:07 +0200\nSubject: Test Email\n\nThis is the body of the email.";
    parse_email_headers(email_data);
    return 0;
}