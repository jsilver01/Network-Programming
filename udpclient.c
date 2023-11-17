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
    int client_socket;
    struct sockaddr_in serv_addr;
    socklen_t serv_addr_size = sizeof(serv_addr);

    client_socket = socket(PF_INET, SOCK_DGRAM, 0);
    if (client_socket == -1)
        error_handling("UDP socket create error");

    memset(&serv_addr, 0, sizeof(serv_addr));
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_addr.s_addr = inet_addr("127.0.0.1"); 
    serv_addr.sin_port = htons(9595);

    FILE *file = fopen("homework.docx", "rb");
    if (!file)
        error_handling("File open error");

    struct packet data_packet;
    data_packet.data_seq = 1;
    int read_size;

    while ((read_size = fread(data_packet.data, 1, 4096, file)) > 0) {
        data_packet.data_size = read_size;

        sendto(client_socket, &data_packet, sizeof(data_packet), 0, (struct sockaddr *)&serv_addr, serv_addr_size);

        printf("%d byte data (seq %d) send.\n", read_size, data_packet.data_seq);

        struct packet ack_packet;
        recvfrom(client_socket, &ack_packet, sizeof(ack_packet), 0, NULL, NULL);
        
        data_packet.data_seq++;
    }

    // 파일 전송 완료를 알리기 위해 0x1A를 전송
    data_packet.data_size = 0x1A;
    sendto(client_socket, &data_packet, sizeof(data_packet), 0, (struct sockaddr *)&serv_addr, serv_addr_size);
    printf("File transfer completed.\n");

    fclose(file);
    close(client_socket);
    return 0;
}

void error_handling(const char *message) {
    fputs(message, stderr);
    fputc('\n', stderr);
    exit(1);
}
