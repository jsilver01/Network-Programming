#include <pthread.h>
#include <stdio.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>

#define MAX_CLIENTS 10
#define PORT 3000
#define BUFFER_SIZE 1024

int client_sockets[MAX_CLIENTS] = {0};
pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;

// 클라이언트 소켓 배열에 추가
void add_client_socket(int client_fd) {
    pthread_mutex_lock(&mutex);
    for (int i = 0; i < MAX_CLIENTS; i++) {
        if (client_sockets[i] == 0) {
            client_sockets[i] = client_fd;
            break;
        }
    }
    pthread_mutex_unlock(&mutex);
}

// 클라이언트 소켓 배열에서 제거
void remove_client_socket(int client_fd) {
    pthread_mutex_lock(&mutex);
    for (int i = 0; i < MAX_CLIENTS; i++) {
        if (client_sockets[i] == client_fd) {
            client_sockets[i] = 0;
            break;
        }
    }
    pthread_mutex_unlock(&mutex);
}

// 클라이언트 메시지 전송 함수
void send_message_to_client(int client_fd, const char *message) {
    pthread_mutex_lock(&mutex);
    write(client_fd, message, strlen(message));
    pthread_mutex_unlock(&mutex);
}

// 모든 클라이언트에게 메시지 전송 함수
void broadcast_message(const char *message) {
    pthread_mutex_lock(&mutex);
    for (int i = 0; i < MAX_CLIENTS; i++) {
        if (client_sockets[i] != 0) {
            write(client_sockets[i], message, strlen(message));
        }
    }
    pthread_mutex_unlock(&mutex);
}

//allbye 입력시 모든 클라이언트 종료 함수
void handle_allbye_command() {
    printf("연결 기다리는 중...\n");
    pthread_mutex_lock(&mutex);
    for (int i = 0; i < MAX_CLIENTS; i++) {
        if (client_sockets[i] != 0) {
            close(client_sockets[i]); // 클라이언트 소켓 종료
            client_sockets[i] = 0;
        }
    }
    pthread_mutex_unlock(&mutex);
}

//클라이언트 핸들링 함수
void *handle_client(void *data) {
    int client_fd = *(int*)data;
    free(data);

    add_client_socket(client_fd);

    char buffer[BUFFER_SIZE];
    while (1) {
        int read_size = read(client_fd, buffer, sizeof(buffer) - 1);
        if (read_size > 0) {
            buffer[read_size] = '\0';
            printf("\n메시지 받음 (클라이언트 %d): %s", client_fd, buffer);
            if (strcmp(buffer, "bye\n") == 0) {
                send_message_to_client(client_fd, "bye\n");
                remove_client_socket(client_fd);
                printf("클라이언트 %d 연결종료\n",client_fd);
                close(client_fd);
                break;
            } else if (strcmp(buffer, "all bye\n") == 0) {
                handle_allbye_command();
                remove_client_socket(client_fd);
                close(client_fd);
                break;
            } else if (strncmp(buffer, "all ", 4) == 0) {
                char *msg = buffer + 4;
                broadcast_message(msg);
                continue;
            }
        } else {
            remove_client_socket(client_fd);
            close(client_fd);
            break;
        }
    }
    return NULL;
}

//서버 입력 핸들링 함수
void *server_input_handler(void *data) {
    char buffer[BUFFER_SIZE];
    while (1) {
        printf("\n메시지 보낼 클라이언트 번호와 메시지를 입력하세요 (예: 2 Hello, all bye): ");
        fgets(buffer, BUFFER_SIZE, stdin);

        if (strcmp(buffer, "all bye\n") == 0) {
            handle_allbye_command();
        }else if (strncmp(buffer, "all ", 4) == 0) {
                char *msg = buffer + 4;
                broadcast_message(msg);
                continue;
        }else{
            int target_client;
            char message[BUFFER_SIZE];
            if(sscanf(buffer, "%d %[^\n]", &target_client, message) == 2){
                send_message_to_client(target_client, message);
            }
        }
    }
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
        perror("bind 실패\n");
        return 1;
    }

    listen(server_fd, MAX_CLIENTS);

    int c = sizeof(struct sockaddr_in);
    printf("연결 기다리는 중...\n");

    // 클라이언트 연결 대기 루프
    while ((client_fd = accept(server_fd, (struct sockaddr *)&client, (socklen_t*)&c))) {
        printf("\n연결된 클라이언트 %d\n", client_fd);

        pthread_t thread;
        int *new_sock = malloc(sizeof(int));
        *new_sock = client_fd;

        if (pthread_create(&thread, NULL, handle_client, (void*)new_sock) < 0) {
            perror("쓰레드 생성 불가");
            return 1;
        }
        // 클라이언트 연결 후 서버 입력 핸들러 스레드 생성
        pthread_t server_thread;
        if (pthread_create(&server_thread, NULL, server_input_handler, NULL) < 0) {
            perror("서버 입력 핸들러 스레드 생성 실패");
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