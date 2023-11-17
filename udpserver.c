#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <unistd.h>

struct packet {
    char data[4096]; //데이터
    int data_size; //데이터 사이즈
    int data_seq; //데이터 시컨스
};

void error_handling(const char *message);

int main() {
    int server_socket;
    struct sockaddr_in serv_addr, client_addr;
    socklen_t client_addr_size = sizeof(client_addr);

    server_socket = socket(PF_INET, SOCK_DGRAM, 0);
    if (server_socket == -1)
        error_handling("UDP socket create error");

    memset(&serv_addr, 0, sizeof(serv_addr));
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_addr.s_addr = htonl(INADDR_ANY);
    serv_addr.sin_port = htons(9595);

    if (bind(server_socket, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) == -1)
        error_handling("bind() error");

    FILE *file = NULL;
    int total_data_received = 0;
    int seq = 1;

    while (1) {
        struct packet received_packet;
        int received_size = recvfrom(server_socket, &received_packet, sizeof(received_packet), 0, (struct sockaddr *)&client_addr, &client_addr_size);
        if (received_size == -1)
            error_handling("recvfrom() error");

        //클라이언트로부터 종료 메시지 받는 경우
        if (received_packet.data_size == 0x1A) {
            printf("File transfer completed.\n");
            break;
        }

        if (!file) {
            file = fopen("received_file", "wb");
            if (!file) {
                error_handling("File open error");
            }
        }

        int write_size = fwrite(received_packet.data, 1, received_packet.data_size, file);
        if (write_size < received_packet.data_size)
            error_handling("File write error");

        total_data_received += received_packet.data_size;
        printf("%d byte data (seq %d) received.\n", received_packet.data_size, received_packet.data_seq);

        struct packet ack_packet;
        ack_packet.data_size = received_packet.data_size;
        ack_packet.data_seq = received_packet.data_seq;
        sendto(server_socket, &ack_packet, sizeof(ack_packet), 0, (struct sockaddr *)&client_addr, client_addr_size);
    }

    fclose(file);
    close(server_socket);
    return 0;
}

void error_handling(const char *message) {
    fputs(message, stderr);
    fputc('\n', stderr);
    exit(1);
}
