#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

#define MAXLINE 1024
#define PORT 1234 

int main() {
    int sockfd, n;
    char buffer[MAXLINE];
    struct sockaddr_in servaddr;

    // 소켓 생성
    if ((sockfd = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
        perror("socket creation failed");
        exit(EXIT_FAILURE);
    }

    // 서버 주소 구조체 설정
    memset(&servaddr, 0, sizeof(servaddr));
    servaddr.sin_family = AF_INET;
    servaddr.sin_port = htons(PORT);
    servaddr.sin_addr.s_addr = inet_addr("127.0.0.1"); // 서버 IP 주소

    // 서버에 연결
    if (connect(sockfd, (struct sockaddr *)&servaddr, sizeof(servaddr)) < 0) {
        perror("connect failed");
        exit(EXIT_FAILURE);
    }

    while (1) {
    printf("Enter a message: ");
    fgets(buffer, MAXLINE, stdin);

    if (buffer[0] == 'q') {
        break;
    }

    write(sockfd, buffer, strlen(buffer));

    n = read(sockfd, buffer, MAXLINE);
    if (n > 0) {
        printf("Server Response: %s\n", buffer);
    }
}

close(sockfd);
return 0;
}
