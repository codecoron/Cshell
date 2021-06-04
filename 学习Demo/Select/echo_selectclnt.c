#include<stdio.h>
#include<arpa/inet.h>
#include<unistd.h>
#include<string.h>
#include<stdlib.h>
#include<sys/select.h>

#define BUF_SIZE 30
void error_handling(char *message);

int main(void){
    int tongxunsock;
    struct sockaddr_in serv_adr;
    fd_set reads;
    fd_set cpy_reads;
    int maxfd;

    

    memset(&serv_adr,0,sizeof(serv_adr));
    serv_adr.sin_family = AF_INET;
    serv_adr.sin_addr.s_addr = inet_addr("127.0.0.1");
    serv_adr.sin_port = htons(9190);

    tongxunsock = socket(PF_INET,SOCK_STREAM,IPPROTO_TCP);

    if(connect(tongxunsock,(struct sockaddr*)&serv_adr,sizeof(serv_adr))==-1)
        error_handling("connect() error\n");

    FD_ZERO(&reads);
    FD_SET(0,&reads);
    FD_SET(tongxunsock,&reads);
    maxfd = tongxunsock;

    struct timeval  time;

    while (1)
    {
        cpy_reads  = reads;
        time.tv_sec = 1;
        time.tv_usec = 0;
        int ret = select(maxfd+1,&cpy_reads,NULL,NULL,&time);
        if(ret ==-1)
            error_handling("select() error\n");
        else if(ret ==0)
            continue;
        else
        {
            //如果用户输入了信息，并且按下了回车，执行这里处理用户输入
            if(FD_ISSET(0,&cpy_reads)){
                char sendbuf[BUF_SIZE];
                fgets(sendbuf, sizeof(sendbuf), stdin);
                 if(!strcmp(sendbuf,"q\n")||!strcmp(sendbuf  ,"Q\n"))
                    break   ;
                send(tongxunsock, sendbuf, strlen(sendbuf), 0); //发送
                 memset(sendbuf, 0, sizeof(sendbuf));
            }
            //如果收到服务端发来的信息，进行处理
            if(FD_ISSET(tongxunsock,&cpy_reads)){
                char recvbuf[BUF_SIZE];
                int len;
                len = recv(tongxunsock, recvbuf, sizeof(recvbuf), 0);
                printf("FROM SERV:%s", recvbuf);
                 memset(recvbuf, 0, sizeof(recvbuf));
            }
        }
    }
    close(tongxunsock);
       
    return 0;
}


void error_handling(char *message){
    printf(message);
    exit(0);
}
/*
客户端利用select，处理键盘输入的信息，和服务端发送过来的信息
*/