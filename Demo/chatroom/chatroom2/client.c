#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<unistd.h>
#include<arpa/inet.h>
#include<sys/socket.h>
#include<sys/types.h>
#include<sys/select.h>

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
    serv_adr.sin_port = htons(9190);

     //初始化自己的UDP绑定地址
    memset(&self_adr,0,sizeof(self_adr));
    self_adr.sin_family = AF_INET;
    self_adr.sin_addr.s_addr = htonl(INADDR_ANY);
     self_adr.sin_port = htons(9999);


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

    fd_set reads;
    fd_set cpy_reads;
    FD_ZERO(&reads);
    FD_SET(0,&reads);
    FD_SET(tcp_sock,&reads);
    FD_SET(mul_sock,&reads);

    int maxfd = mul_sock;

    while (1)
    {
        struct   timeval time;
        time.tv_sec = 1;
        time.tv_usec = 0;

         cpy_reads = reads;
        int ret  = select(maxfd+1,&cpy_reads,NULL,NULL,&time);
        if(ret == -1) 
            error_handling("select error()\n");
        else if(ret ==0)
            continue;
        else {
            if(FD_ISSET(0,&cpy_reads))
                {
                    char buf[BUF_SIZE];
                    memset(buf,0,BUF_SIZE);
                    //scanf("%s",buf);//scanf不会读进换行和空格
                    read(0,buf,BUF_SIZE-1);
                    write(tcp_sock,buf,strlen(buf));
                }
            if(FD_ISSET(tcp_sock,&cpy_reads))
                ;
            if(FD_ISSET(mul_sock,&cpy_reads))
                {
                    char buf[BUF_SIZE];
                    memset(buf,0,sizeof(buf));
                    recvfrom(mul_sock,buf,BUF_SIZE,0,NULL,0);
                    write(1,"recv:",sizeof("recv:"));
                    write(1,buf,sizeof(buf));
                }
        }

    }

    close(mul_sock);
    close(tcp_sock);
    return 0;
}

void error_handling(char *message){
    printf(message);
    exit(0);
}