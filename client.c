#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/types.h>
#include <sys/socket.h>

#define BUFF_SIZE 1024

int main(int argc, char **argv) {
    int client_socket;
    struct sockaddr_in server_addr;
    char buff[BUFF_SIZE + 5]; //전체사이즈는 1029

    client_socket = socket(PF_INET, SOCK_STREAM, 0);
    if (-1 == client_socket) {
        printf("socket create failed\n");
        exit(1);
    }

    memset(&server_addr, 0, sizeof(server_addr));
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(4000);
    server_addr.sin_addr.s_addr = inet_addr("127.0.0.1"); //내 자신 pc의 주소를 말하는거임

    if (-1 == connect(client_socket, (struct sockaddr *)&server_addr, sizeof(server_addr))) {
        printf("connect failed\n");
        exit(1);
    }

    while (1) {
        printf("Client: ");
        fgets(buff, BUFF_SIZE, stdin);
        buff[strcspn(buff, "\n")] = '\0';

        write(client_socket, buff, strlen(buff) + 1);

        if (strcmp(buff, "q") == 0) {
            printf("client terminated\n");
            break;
        }

        read(client_socket, buff, BUFF_SIZE);
        printf("Server: %s\n", buff);
				if(strcmp(buff,"q")==0){
					printf("server terminated\n");
					break;
				}
    }

    close(client_socket);

    return 0;
}