#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<unistd.h>
#include<arpa/inet.h>
#include<sys/socket.h>

#define BUF_SIZE 60

void error_handling(char *message);

int main(int argc,char *argv[]){
    
    int recv_sock;
    int str_len;
    char buf[BUF_SIZE];
    struct sockaddr_in adr;
    struct ip_mreq join_adr;

//     if(argc!=3)
//    error_handling("Usage :<Group> <PORT>\n");

    recv_sock = socket(AF_INET,SOCK_DGRAM,IPPROTO_UDP);
    if(recv_sock==-1) error_handling("socket() error\n");

    memset(&adr,0,sizeof(adr));
    adr.sin_family = AF_INET;
    adr.sin_addr.s_addr = htonl(INADDR_ANY);
    adr.sin_port = htons(9999);

    int optval = 1;
    if(setsockopt(recv_sock,SOL_SOCKET,SO_REUSEADDR,(void *)&optval,sizeof(optval))==-1)
        error_handling("setsockopt reuseAddr error\n");
	if(bind(recv_sock,(struct sockaddr*)&adr,sizeof(adr))==-1) 
        error_handling("bind() error\n");
    
    join_adr.imr_multiaddr.s_addr = inet_addr("224.1.1.2");
    join_adr.imr_interface.s_addr=htonl(INADDR_ANY);

    if(setsockopt(recv_sock,IPPROTO_IP,IP_ADD_MEMBERSHIP,(void *)&join_adr,sizeof(join_adr))==-1)
        error_handling("setsockopt() error");
    
    int count = 0;
    while(1){
        str_len = recvfrom(recv_sock,buf,BUFSIZ,0,NULL,0);
        if(str_len<0) break;
        if(buf[str_len-1]!='\n')
             buf[str_len-1] = '\n'; 
        fputs(buf,stdout);
    }
    close(recv_sock);
    return 0;
}

void error_handling(char *message){
    printf(message);
    exit(0);
}

