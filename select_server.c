#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/time.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

#define SERVER_SOCKET_SELECT_ERR -1
#define SERVER_SOCKET_TIMEOUT -2
#define MAXLINE 1024

int main(void) {
    int listenfd, connfd, state, ret;
    struct sockaddr_in servaddr, cliaddr;
    socklen_t cliaddr_len;
    char buffer[MAXLINE];
    fd_set readfds;
    struct timeval tv;
    ssize_t recv_len; // 수신된 데이터 길이

    int in_socket_server_client_fd; // 클라이언트 소켓 파일 디스크립터
    int in_timeout = 10; // 타임아웃 시간 (초)

    // 소켓 생성
    listenfd = socket(AF_INET, SOCK_STREAM, 0);

    // 서버 주소 구조체 설정
    memset(&servaddr, 0, sizeof(servaddr));
    servaddr.sin_family = AF_INET;
    servaddr.sin_addr.s_addr = htonl(INADDR_ANY);
    servaddr.sin_port = htons(1234);

    // 소켓에 주소 할당
    bind(listenfd, (struct sockaddr *) &servaddr, sizeof(servaddr));

    // 연결 요청 대기
    listen(listenfd, 10);

    // 클라이언트 연결 수락
    cliaddr_len = sizeof(cliaddr);
    connfd = accept(listenfd, (struct sockaddr *) &cliaddr, &cliaddr_len);
    in_socket_server_client_fd = connfd;

   // ... [서버 초기화 코드] ...

while (1) {
    // fd_set 초기화 및 클라이언트 소켓 설정
    FD_ZERO(&readfds);
    FD_SET(in_socket_server_client_fd, &readfds);

    // 타임아웃 설정
    tv.tv_sec = in_timeout;
    tv.tv_usec = 0;

    // select를 사용하여 소켓의 상태 확인
    state = select(in_socket_server_client_fd + 1, &readfds, NULL, NULL, &tv);

    switch(state) {
        case -1: // select 함수 에러
            printf("select error\n");
            ret = SERVER_SOCKET_SELECT_ERR;
            break;
        
        case 0: // 타임아웃 발생
            printf("Time out error\n");
            ret = SERVER_SOCKET_TIMEOUT;
            close(connfd);
            close(listenfd);    
            
            break;

        default: // 데이터 수신 가능 상태
            if (FD_ISSET(in_socket_server_client_fd, &readfds)) {
                memset(buffer, 0, MAXLINE);
                recv_len = read(in_socket_server_client_fd, buffer, MAXLINE);
                if (recv_len > 0) {
                    printf("Received: %s\n", buffer); // 수신된 데이터 출력

                    // 'q' 수신 시 연결 종료
                    if (buffer[0] == 'q') {
                        close(in_socket_server_client_fd);
                        return 0;
                    }

                    // 클라이언트에 에코 메시지 보내기
                    write(in_socket_server_client_fd, buffer, recv_len);
                }
            }
            break;
    }
}

// 연결 종료
close(connfd);
close(listenfd);

return ret;

}
