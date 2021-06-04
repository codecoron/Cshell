#include<stdio.h>
#include<stdlib.h>
#include<fcntl.h>
#include<unistd.h>
#include<sys/socket.h>
void error_handling(char *message);
int main(void){
    int fd;
    char buf[] = "let's go!\n";
    fd = open("data.txt",O_CREAT|O_WRONLY|O_TRUNC);
    if(fd==-1) error_handling("open() error\n");
    
    printf("file descriptor:%d\n",fd);

    if(write(fd,buf,sizeof(buf))==-1) error_handling("open() error\n");
    close(fd);
    return 0;
}
void error_handling(char *message){
    printf(message);
    exit(0);
}