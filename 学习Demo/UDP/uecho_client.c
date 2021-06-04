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

    sock = socket(PF_INET,SOCK_DGRAM,IPPROTO_UDP);
    if(sock==-1) error_handling("socket() error");

    memset(&serv_adr,0,sizeof(serv_adr));
    serv_adr.sin_family = AF_INET;
    //serv_adr.sin_addr.s_addr=htonl(INADDR_ANY);
    serv_adr.sin_addr.s_addr=inet_addr("127.0.0.1");
    serv_adr.sin_port = htons(9190);

    while (1)
    {
        fputs("Insert message(q to quit): /",stdout);
        fgets(message,sizeof(message),stdin);
        if(!strcmp(message,"q\n")||!strcmp(message  ,"Q\n"))
            break   ;
        sendto(sock ,message,BUF_SIZE,0,(struct sockaddr*)&serv_adr ,sizeof(serv_adr));
        adr_sz = sizeof(from_adr);
        str_len = recvfrom(sock,message,BUF_SIZE,0,(struct sockaddr*)&from_adr,&adr_sz);
        message[str_len] = 0;
        printf("Message from server :%s\n",message);
    }
    
    close(sock);
    return 0;
}

void error_handling(char *message){
    printf(message);
    exit(0);
}
