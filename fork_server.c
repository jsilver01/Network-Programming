#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <signal.h>
#include <sys/wait.h>

#define BUF_SIZE 30
void error_handling(char* message);
void read_childproc(int sig);

int main(int argc, char* argv[])
{
   int serv_sock, clnt_sock;
   struct sockaddr_in serv_adr, clnt_adr;

   pid_t pid;
   socklen_t adr_sz;
   int str_len, state;
   char buf[BUF_SIZE];
   if(argc != 2)
   {
      printf("Usage : %s <port> \n", argv[0]);
      exit(1);
   }
   serv_sock = socket(PF_INET, SOCK_STREAM, 0);
   memset(&serv_adr, 0, sizeof(serv_adr));
   serv_adr.sin_family = AF_INET;
   serv_adr.sin_addr.s_addr = htonl(INADDR_ANY);
   serv_adr.sin_port = htons(atoi(argv[1]));

   if(bind(serv_sock, (struct sockaddr*) &serv_adr, sizeof(serv_adr)) == -1)
      error_handling("bind() error");

   if(listen(serv_sock, 5) == -1)
      error_handling("listen() error");
   while(1)
   {
      adr_sz = sizeof(clnt_adr);
      clnt_sock = accept(serv_sock, (struct sockaddr*)&clnt_adr, &adr_sz); // accept 함수 호출한 후에 밑의 fork 함수 호출
                                 // 때문에 이 구문에서 만들어진 소켓(클라이언트의 연결요청 수락과정에서 만들어진) 파일 디스크립터를
                                 // 부모 프로세스와 자식 프로세스가 동시에 하나씩 갖게 됨
      if(clnt_sock == -1)
         continue;
      else
         puts("new client connected....");
      pid = fork();
      if(pid == -1)
      {
         close(clnt_sock);
         continue;
      }
      if(pid == 0)  // 자식 프로세스 실행 영역
                    // 자식 프로세스가 실행되면서 클라이언트에게 에코 서비스가 제공됨.
                    // 그런데 close로 위의 socket을 닫고있는데 이는 자식 프로세스로 서버 소켓의 파일 디스크립터까지 복사되기 때문이다.
      {
         close(serv_sock);  // 자식 영역에서는 서버 소켓을 닫음(필요없는 소켓을 열어둘필요가 없기때문)
         while((str_len = read(clnt_sock, buf, BUF_SIZE)) != 0)
            write(clnt_sock, buf, str_len);  // 클라이언트 소켓을통해 데이터 주고받음

         //close(clnt_sock);                   // 클라이언트 소켓 닫음
         puts("client disconnected...");
         return 0;
      }
    //   else
    //      close(clnt_sock);  // accept 함수 호출을 통해 만들어진 소켓의 파일 디스크립터가 자식 프로세스에게 복사되었으니
                            // 서버는 자신이 소유하고있는 파일 디스크립터를 소멸시켜야함.
   }
   close(serv_sock);        // 부모 프로세스에서 모든 클라이언트와의 연결이 끝났으니 서버소켓을 닫아줌
   return 0;
}

void read_childproc(int sig)
{
   pid_t pid;
   int status;
   pid = waitpid(-1, &status, WNOHANG);
   printf("removed proc id : %d \n", pid);
}

void error_handling(char* message)
{
    fputs(message, stderr);
    fputc('\n', stderr);
    exit(1);
}