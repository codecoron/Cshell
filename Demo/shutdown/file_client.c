#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<unistd.h>
#include<arpa/inet.h>
#include<sys/socket.h>

#define BUF_SIZE 30
void error_handling(char *message);

int main(void){

    int sd;
    FILE *fp;

    char buf[BUF_SIZE];
    int read_cnt;
    struct sockaddr_in serv_adr;

    fp = fopen("receive.dat","wb");
    if(fp==NULL) error_handling("fopen() error\n");

    sd = socket(PF_INET,SOCK_STREAM,IPPROTO_TCP);
    if(sd==-1) error_handling("socket() error\n");

    memset(&serv_adr,0,sizeof(serv_adr));
    serv_adr.sin_addr.s_addr = inet_addr("127.0.0.1");
    serv_adr.sin_family = AF_INET;
    serv_adr.sin_port = htons(9190);

    if(connect(sd,(struct sockaddr*)&serv_adr,sizeof(serv_adr))) 
        error_handling("connect() error\n");

    while((read_cnt=read(sd,buf,BUF_SIZE))!=0) 
        fwrite(buf,1,read_cnt,fp);

    puts("Received file data");
    write(sd,"Thank you ",10);
    fclose(fp);
    close(sd);

    return 0;
}

void error_handling(char *message){
    printf(message);
    exit(0);
}