#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>

int main() {
    int client_socket;
    struct sockaddr_in server_addr;
    char message[] = "hi client";
    char buffer[1024];

    client_socket = socket(AF_INET, SOCK_STREAM, 0);
    if (client_socket < 0) {
        perror("socket");
        exit(1);
    }

    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(4000);
    server_addr.sin_addr.s_addr = inet_addr("127.0.0.1");

    if (connect(client_socket, (struct sockaddr*)&server_addr, sizeof(server_addr)) < 0) {
        perror("connect");
        exit(1);
    }
    while(1){
        
    }
    send(client_socket, message, strlen(message), 0);
    printf("Sent to server: %s\n", message);

    recv(client_socket, buffer, sizeof(buffer), 0);
    printf("Received from server: %s\n", buffer);

    close(client_socket);

    return 0;
}
