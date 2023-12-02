#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

#define SERVER "127.0.0.1"
#define PORT 3000

int main() {
    struct sockaddr_in server_addr;
    int sock, read_size;
    char *message = "start\n";
    char server_reply[2000];

    sock = socket(AF_INET, SOCK_STREAM, 0);
    if (sock == -1) {
        printf("소켓 생성불가");
        return 1;
    }
    printf("소켓 생성\n");

    server_addr.sin_addr.s_addr = inet_addr(SERVER);
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(PORT);

    if (connect(sock, (struct sockaddr *)&server_addr, sizeof(server_addr)) < 0) {
        perror("connect 실패");
        return 1;
    }

    printf("서버 연결 완료!\n");

    if (send(sock, message, strlen(message), 0) < 0) {
        printf("send 실패");
        return 1;
    }

    while ((read_size = recv(sock, server_reply, sizeof(server_reply) - 1, 0)) > 0) {
        server_reply[read_size] = '\0';
        printf("%s", server_reply);
        memset(server_reply, 0, sizeof(server_reply));
    }

    if (read_size == 0) {
        printf("서버 연결 종료\n");
    } else if (read_size == -1) {
        perror("recv 실패");
    }

    close(sock);
    return 0;
}
