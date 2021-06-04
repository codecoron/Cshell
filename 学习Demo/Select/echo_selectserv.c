#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<unistd.h>
#include<arpa/inet.h>
#include<sys/socket.h>
#include<sys/select.h>

#define BUF_SIZE 30
void error_handling(char *messgae);

int main(int argc,char *argv[]){
    int serv_sock;
    int tongxunsock;

    struct sockaddr_in serv_adr;
    struct sockaddr_in clnt_adr;

    fd_set reads;
    fd_set cpy_reads;

    socklen_t adr_sz;

    int  fd_max;
    int fd_num;
    int str_len;
    char buf[BUF_SIZE];

    serv_sock = socket(PF_INET,SOCK_STREAM,IPPROTO_TCP);
    memset(&serv_adr,0,sizeof(serv_adr));
    serv_adr.sin_family = AF_INET;
    serv_adr.sin_addr.s_addr = htonl(INADDR_ANY);
    serv_adr.sin_port = htons(9190);

    if(bind(serv_sock,(struct sockaddr*)&serv_adr,sizeof(serv_adr))==-1) 
        error_handling("bind() errror\n");
    if(listen(serv_sock,20)==-1) 
        error_handling("listen() error\n");

    FD_ZERO(&reads);
    FD_SET(serv_sock,&reads);
    fd_max = serv_sock;
    printf("fd_max = %d\n",fd_max);

    while(1){
        cpy_reads = reads;

        if((fd_num=select(fd_max+1,&cpy_reads,0,0,0))==-1)
            break;
        if(fd_num ==0) continue;

        for(int i=0;i<fd_max+1;i++){
            if(FD_ISSET(i,&cpy_reads)) {
                //有新的连接请求
                if(i==serv_sock) {
                    tongxunsock = accept(serv_sock,(struct sockaddr*)&clnt_adr,&adr_sz);
                    FD_SET(tongxunsock,&reads);
                    if(fd_max<tongxunsock) fd_max = tongxunsock;
                        printf("connected client :%d\n",tongxunsock);
                }
                //其他通讯套接字有变化
                else {
                    memset(buf,0,BUF_SIZE);
                    str_len = read(i,buf,BUF_SIZE);
                    //断开连接
                    if(str_len==0){
                         printf("Client [%d] 断开连接\n",i);
                         FD_CLR(i,&reads);
                         close(i);
                    }
                    //返回数据
                    else {
                        write(i,buf,strlen(buf));
                        printf("%s",buf);
                    }
                }
            }        
        }
    }

    close(serv_sock);
    return 0;
}

void error_handling(char *message){
    fputs(message,stderr);
    fputs("\n",stderr);
    exit(1);
}

/*
只允许在 C99 模式下使用‘for’循环初始化声明
如何查看我的编译器用哪个标准？
*/