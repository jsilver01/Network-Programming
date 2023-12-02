#include <stdio.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>

#define PORT 3000
#define BUFFER_SIZE 1024

int main() {
    int sock;
    struct sockaddr_in server;
    char message[BUFFER_SIZE], server_reply[BUFFER_SIZE];

    sock = socket(AF_INET, SOCK_STREAM, 0);
    if (sock == -1) {
        printf("소켓 생성 실패\n");
        return 1;
    }

    server.sin_family = AF_INET;
    server.sin_addr.s_addr = INADDR_ANY;
    server.sin_port = htons(PORT);

    if (connect(sock, (struct sockaddr *)&server, sizeof(server)) < 0) {
        perror("connect 실패");
        return 1;
    }

    printf("서버와 연결되었습니다.\n");

    while (1) {
        printf("메시지 입력: ");
        fgets(message, BUFFER_SIZE, stdin);

        if (send(sock, message, strlen(message), 0) < 0) {
            puts("메시지 전송 실패");
            continue; // 메시지 전송 실패 시 계속 시도
        }

        if (strcmp(message, "bye\n") == 0) {
            break;
        }

        // 서버로부터의 응답 대기
        int bytes_received = recv(sock, server_reply, BUFFER_SIZE, 0);
        if (bytes_received > 0) {
            server_reply[bytes_received] = '\0';
            printf("서버 응답: %s\n", server_reply);
        } else if (bytes_received == 0) {
            puts("서버 연결 종료");
            break;
        } else {
            puts("응답 수신 실패");
            break;
        }
    }

    close(sock);
    return 0;
}
