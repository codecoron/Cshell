#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<unistd.h>
#include<arpa/inet.h>
#include<sys/socket.h>

#define BUF_SIZE 30
void error_handling(char *message);

int main(int argc,char *argv[]){
    int serv_sock;
    char message[BUF_SIZE];
    int str_len;
    socklen_t clnt_adr_sz;
    
    struct sockaddr_in serv_adr;
    struct sockaddr_in clnt_adr;

    //这里的第三个参数可以设置为0，自动选择何种报文
    serv_sock = socket(PF_INET,SOCK_DGRAM,IPPROTO_UDP);
    if(serv_sock==-1) error_handling("socket() error\n");

    memset(&serv_adr,0,sizeof(serv_adr));
    serv_adr.sin_family = AF_INET;
    serv_adr.sin_addr.s_addr=htonl(INADDR_ANY);
    serv_adr.sin_port = htons(9190);

    if(bind(serv_sock,(struct sockaddr *)&serv_adr  ,sizeof(serv_adr))==-1) 
        error_handling("bind() error\n");
    
    while (1)
    {   
        clnt_adr_sz = sizeof(clnt_adr);
        str_len = recvfrom(serv_sock,message,BUF_SIZE,0,(struct sockaddr*)&clnt_adr,&clnt_adr_sz);
        sendto(serv_sock,message,str_len,0,(struct sockaddr*)&clnt_adr,clnt_adr_sz);
    }
    close(serv_sock);
    return 0;
}

void error_handling(char *message){
    printf(message);
    exit(0);
}
