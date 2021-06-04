#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<unistd.h>
#include<arpa/inet.h>
#include<sys/socket.h>

#define BUF_SIZE 1024
void error_handling(char *messgae);

int main(void){

    int sock;
    struct sockaddr_in serv_adr;
    char buf[BUF_SIZE] = {0,};

    memset(&serv_adr,0,sizeof(serv_adr));
    serv_adr.sin_family = AF_INET;
    serv_adr.sin_addr.s_addr = inet_addr("127.0.0.1");
    serv_adr.sin_port = htons(9190);

    sock = socket(PF_INET,SOCK_STREAM,IPPROTO_TCP);
    if(connect(sock,(struct sockaddr*)&serv_adr,sizeof(serv_adr))==-1)
        error_handling("connet() error\n");

    while (read(sock,buf,BUF_SIZE)>0)
    {
        puts(buf);
    }
    write(sock,"Bye Server\n",sizeof("Bye Server\n"));
    close(sock);
    return 0;
}

void error_handling(char *message){
    fputs(message,stderr);
    fputs("\n",stderr);
    exit(1);
}