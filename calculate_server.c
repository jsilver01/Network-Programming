#include <pthread.h>
#include <stdio.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>

#define MAX_CLIENTS 10
#define PORT 3000

pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;

void print_table(int client_fd) {
    char buffer[1024];
    for (int i = 2; i <= 9; i++) {
        for (int j = 1; j <= 9; j++) {
            pthread_mutex_lock(&mutex);
            snprintf(buffer, sizeof(buffer), "클라이언트 %d: %d * %d = %d\n", client_fd, i, j, i*j);
            write(client_fd, buffer, strlen(buffer));
            pthread_mutex_unlock(&mutex);
        }
    }
}

void *handle_client(void *data) {
    int client_fd = *(int*)data;
    free(data); 

    char buffer[1024];
    while (1) {
        int read_size = read(client_fd, buffer, sizeof(buffer) - 1);
        if (read_size > 0) {
            buffer[read_size] = '\0'; 
            if (strcmp(buffer, "start\n") == 0) {
                print_table(client_fd);
                break; 
            }
        } else {
            break; 
        }
    }
    
    close(client_fd);
    return NULL;
}

int main() {
    int server_fd, client_fd;
    struct sockaddr_in server, client;
    
    server_fd = socket(AF_INET, SOCK_STREAM, 0);
    if (server_fd == -1) {
        printf("소켓 생성 실패\n");
        return 1;
    }

    server.sin_family = AF_INET;
    server.sin_addr.s_addr = INADDR_ANY;
    server.sin_port = htons(PORT);

    if (bind(server_fd, (struct sockaddr *)&server, sizeof(server)) < 0) {
        perror("bind 실패");
        return 1;
    }

    listen(server_fd, MAX_CLIENTS);

    printf("연결 기다리는중...\n");
    int c = sizeof(struct sockaddr_in);

    while ((client_fd = accept(server_fd, (struct sockaddr *)&client, (socklen_t*)&c))) {
        printf("연결된 클라이언트 %d\n", client_fd);
        
        pthread_t thread;
        int *new_sock = malloc(sizeof(int));
        *new_sock = client_fd;
        
        if (pthread_create(&thread, NULL, handle_client, (void*)new_sock) < 0) {
            perror("쓰레드 생성 불가");
            return 1;
        }
    }

    if (client_fd < 0) {
        perror("accept 실패");
        return 1;
    }

    pthread_mutex_destroy(&mutex);
    return 0;
}
