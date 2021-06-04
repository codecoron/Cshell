#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<unistd.h>
#include<arpa/inet.h>
#include<sys/socket.h>

#define TTL 640
#define BUF_SIZE 60

void error_handling(char *message);

int main(int argc ,char *argv[]){
    
    int send_sock;
    struct sockaddr_in mul_adr;
    int time_ive = TTL;
    FILE *fp;
    char buf[BUF_SIZE];

    // if(argc!=3)
    // error_handling("Usage :<GroupIP> <PORT> \n");

    send_sock = socket(PF_INET,SOCK_DGRAM,0);
    
    memset(&mul_adr,0,sizeof(mul_adr));
    mul_adr.sin_family = AF_INET;
    mul_adr.sin_addr.s_addr = inet_addr("224.1.1.2");
    mul_adr.sin_port = htons(9999);

    //setsockopt(send_sock,IPPROTO_IP,IP_MULTICAST_TTL,NULL,0);

    if((fp = fopen("news.txt","r"))==NULL) 
        error_handling("fopen() error\n");

    while(!feof(fp)){
        fgets(buf,BUF_SIZE,fp);
        sendto(send_sock,buf,strlen(buf),0,(struct sockaddr*)&mul_adr,sizeof(mul_adr));
        fputs(buf,stdout);
        sleep(1);
    }
    fclose(fp);
    close(send_sock);
    return 0;
}


void error_handling(char *message){
    printf(message);
    exit(0);
}