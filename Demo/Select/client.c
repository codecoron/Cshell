#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<unistd.h>
#include<arpa/inet.h>
#include<sys/socket.h>

#define BUF_SIZE 30
void error_handling(char *message);

int main(int argc,char *argv[]){

    int sock;
    char message[BUF_SIZE];
    int str_len;
    socklen_t adr_sz;

    struct sockaddr_in serv_adr;
    struct sockaddr_in from_adr;

    sock = socket(PF_INET,SOCK_STREAM,IPPROTO_IP);
    if(sock==-1) error_handling("socket() error");

    memset(&serv_adr,0,sizeof(serv_adr));
    serv_adr.sin_family = AF_INET;
    //serv_adr.sin_addr.s_addr=htonl(INADDR_ANY);
    serv_adr.sin_addr.s_addr=inet_addr("127.0.0.1");
    serv_adr.sin_port = htons(9190);

    if(connect(sock,(struct sockaddr*)&serv_adr,sizeof(serv_adr))==-1) 
        error_handling("connect() error\n");
    else printf("connect...\n");

    while (1)
    {
        fputs("Insert message(q to quit):",stdout);
        fgets(message,sizeof(message),stdin);
        if(!strcmp(message,"q\n")||!strcmp(message  ,"Q\n"))
            break   ;
        write(sock,message,strlen(message));
        str_len = read(sock,message,BUF_SIZE-1);
        message[str_len] = 0;
        printf("Message from server :%s",message);
    }
    
    close(sock);
    return 0;
}

void error_handling(char *message){
    printf(message);
    exit(0);
}

/*
输入一条信息，发送一条信息
*/
