#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<unistd.h>
#include<arpa/inet.h>
#include<sys/socket.h>

#define BUF_SIZE 1024
void error_handling(char *messgae);


int main(int argc,char *argv[]){

    int serv_sock ;
    int tongxunsock;
    FILE *readfp;
    FILE *writefp;

    struct sockaddr_in serv_adr;
    char buf[BUF_SIZE] = {0,};
    
    serv_sock = socket(PF_INET,SOCK_STREAM,IPPROTO_TCP);
    memset(&serv_adr,0,sizeof(serv_adr));
    serv_adr.sin_family = AF_INET;
    serv_adr.sin_addr.s_addr = inet_addr("127.0.0.1");
    serv_adr.sin_port  = htons(9190);

    if(bind(serv_sock,(struct sockaddr*)&serv_adr,sizeof(serv_adr))==-1)
        error_handling("bind() error\n");

    if(listen(serv_sock,5)==-1)
        error_handling("listen() error\n");

    tongxunsock = accept(serv_sock,NULL,0);

    readfp = fdopen(tongxunsock,"r");
    writefp = fdopen(tongxunsock,"w");

    fputs("FROM SERVER:Hi client\n",writefp);
    fputs("I love all of the world\n",writefp);
    fputs("You are awesome!\n",writefp);
    fflush(writefp);

    shutdown(fileno(writefp),SHUT_WR);
    fclose(writefp);

    fgets(buf,sizeof(buf),readfp);
    fputs(buf,stdout);
    fclose(readfp);

    return 0;
}

void error_handling(char *message){
    fputs(message,stderr);
    fputs("\n",stderr);
    exit(1);
}