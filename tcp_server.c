#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>

#define BUFF_SIZE 1024
#define MAX_DATA_SIZE 255

struct Packet {
    char cmd;
    int len;
    char data[MAX_DATA_SIZE];
};

int main(int argc, char **argv){
    int server_socket;
    int client_socket;
    socklen_t client_addr_size;
    struct sockaddr_in server_addr;
    struct sockaddr_in client_addr;
    
    server_socket = socket(AF_INET, SOCK_STREAM, 0);
    if(-1 == server_socket){
        printf("ERROR opening socket");
        exit(1);
    }

    memset(&server_addr, 0, sizeof(server_addr));
    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = htonl(INADDR_ANY);
    server_addr.sin_port = htons(9595);

    if (-1 == bind(server_socket, (struct sockaddr *)&server_addr, sizeof(server_addr))){
        printf("ERROR on binding");
        exit(1);
    }

    if(-1 == listen(server_socket, 5)){
        printf("ERROR on listening");
        exit(1);
    }

    while(1){
        client_socket = accept(server_socket, (struct sockaddr *)&client_addr, &client_addr_size);
        if(-1 == client_socket){
            printf("ERROR on accept");
            continue;
        }

        struct packet packet;
        while(1){
            if(-1 == read()){
                printf("ERROR reading from socket");
                break;
            }
        }   

    }
}