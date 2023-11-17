#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <sys/types.h>

#define BUFF_SIZE 1024

int main(int argc, char **argv){
    int server_socket = 0;
    int client_socket = 0;
    int client_addr_size = 0;
    struct sockaddr_in server_addr;
    struct sockaddr_in client_addr;
    char buff_rcv[BUFF_SIZE];
    char buff_snd[BUFF_SIZE];

    memset(&server_addr , 0x00, sizeof(server_addr));
    memset(&client_addr, 0x00, sizeof(client_addr));

    server_socket = socket(PF_INET, SOCK_STREAM,0);
    if(-1 == server_socket){
        printf("실패");
        exit(1);
    }

    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(4000);
    server_addr.sin_addr.s_addr = htonl(INADDR_ANY);

    if(-1 == bind(server_socket, (struct sockaddr *)&server_addr, sizeof(server_addr))){
        printf("bind 실패");
        exit(1);
    }
    
    if(-1 == listen(server_socket, 5)){
        printf("listen 실패");
        exit(1);
    }

    while(1){
        client_addr_size = sizeof(client_addr);
        client_socket = accept(server_socket, (struct sockaddr *)&client_addr, &client_addr_size);
        if(-1 == client_socket){
            printf("client 연결 실패");
            exit(1);
        }
        while(1){
            read(client_socket, buff_rcv, BUFF_SIZE);
            printf("client : %s", buff_rcv);

            if(strcmp(buff_rcv,"q")==0){
                printf("client 종료");
                close(client_socket);
                exit(1);
            }
            printf("server : ");
            fgets(buff_snd, BUFF_SIZE, stdin);
            write(client_socket, buff_snd,strlen(buff_snd)+1);
        }
    }
    close(server_socket);
    return 0;
}