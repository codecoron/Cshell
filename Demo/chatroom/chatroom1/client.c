#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<unistd.h>
#include<arpa/inet.h>
#include<sys/socket.h>
#include<sys/types.h>

#define BUF_SIZE 30
void error_handling(char *message);
void write_routine(int tcp_sock,char *buf);
void read_routine(int tcp_sock,char*buf);
void receive_mul(int tcp_sock,char *buf);
int main(int argc,char *argv[]){

    int tcp_sock;
    int mul_sock;
    pid_t pid;
    char message[BUF_SIZE];
    socklen_t adr_sz;

    struct sockaddr_in serv_adr;//服务器地址
    struct sockaddr_in from_adr;
    struct sockaddr_in self_adr;//自己的UDP绑定地址
    struct ip_mreq join_adr;//多播地址

    //if(argc!=3) error_handling("Usage<YourIpt> <YouPort>\n");

    tcp_sock = socket(PF_INET,SOCK_STREAM,IPPROTO_TCP);
    if(tcp_sock==-1) error_handling("socket() error");

    mul_sock = socket(PF_INET,SOCK_DGRAM,IPPROTO_UDP);
    if(mul_sock==-1) error_handling("socket() error");

    //初始化要连接的服务器地址
    memset(&serv_adr,0,sizeof(serv_adr));
    serv_adr.sin_family = AF_INET;
    serv_adr.sin_addr.s_addr=htonl(INADDR_ANY);
    printf("INADDR_ANY = %X\n",serv_adr.sin_addr.s_addr);
    //serv_adr.sin_addr.s_addr=inet_addr("127.0.0.1");
    serv_adr.sin_port = htons(9190);

     //初始化自己的UDP绑定地址
    memset(&self_adr,0,sizeof(self_adr));
    self_adr.sin_family = AF_INET;
    self_adr.sin_addr.s_addr = htonl(INADDR_ANY);
  // self_adr.sin_addr.s_addr = inet_addr("192.168.187.135");
     self_adr.sin_port = htons(9999);
    //self_adr.sin_port = htons(atoi(argv[2]));

    long optval = 1;
    if(setsockopt(mul_sock,SOL_SOCKET,SO_REUSEADDR,(void *)&optval,sizeof(optval))==-1){
        error_handling("setsockopt() error\n");
    }
     if(bind(mul_sock,(struct sockaddr*)&self_adr,sizeof(self_adr))==-1) 
        error_handling("bind() error\n");
    
    //初始化要加入的多播组地址
    join_adr.imr_multiaddr.s_addr = inet_addr("224.1.1.2");
    //join_adr.imr_interface.s_addr=htonl(INADDR_ANY);
    join_adr.imr_interface.s_addr=htonl(INADDR_ANY);

    if(setsockopt(mul_sock,IPPROTO_IP,IP_ADD_MEMBERSHIP,(void *)&join_adr,sizeof(join_adr))==-1)
        error_handling("setsockopt() error");

    if(connect(tcp_sock,(struct sockaddr*)&serv_adr,sizeof(serv_adr))==-1) 
        error_handling("connect() error\n");
    else printf("connect...\n");

    pid =fork();
    if(pid == 0){
        receive_mul(mul_sock,message);
    }
    else {
        write_routine(tcp_sock,message);     
    }
    close(mul_sock);
    close(tcp_sock);
    return 0;
}

void error_handling(char *message){
    printf(message);
    exit(0);
}

void receive_mul(int tcp_sock,char *buf){
        while(1){
        memset(buf,0,BUF_SIZE);
        int  str_len = recvfrom(tcp_sock,buf,BUFSIZ,0,NULL,0);
        if(str_len<0) break;
        else printf("recv:");
        if(buf[str_len-1]!='\n'){
            buf[str_len-1] = '\n'; 
        }
        fputs(buf,stdout);
    }
}


void read_routine(int tcp_sock,char*buf){
    while(1){
        memset(buf,0,BUF_SIZE);
        int str_len;
        str_len = read(tcp_sock,buf,BUF_SIZE-1);
        if(str_len==0) {
            printf("str_len ==0\n");
            return ;
        } 
        buf[str_len] = 0;
        printf("Message from server :%s",buf);
    }
}


void write_routine(int tcp_sock,char *buf){
    while (1)
    {
        memset(buf,0,sizeof(buf));
        fgets(buf,sizeof(buf),stdin);
        if(!strcmp(buf,"q\n")||!strcmp(buf  ,"Q\n")){
            shutdown(tcp_sock,SHUT_WR);
            return;
        }
        write(tcp_sock,buf,strlen(buf));
    }
}
