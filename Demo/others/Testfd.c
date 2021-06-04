#include<stdio.h>
#include<fcntl.h>
#include<unistd.h>
#include<sys/socket.h>
int main(void){
    int fd1;
    int fd2;
    int fd3;
    int fd4;
    fd1 = socket(PF_INET,SOCK_STREAM,0);
    fd2 = open("data.txt",O_CREAT|O_WRONLY|O_TRUNC);
    fd3 = socket(PF_INET,SOCK_STREAM,0);

    printf("file descriptor 1:%d\n",fd1);
    printf("file descriptor 2:%d\n",fd2);
    printf("file descriptor 3:%d\n",fd3);
    
    close(fd2);
    fd4 = socket(PF_INET,SOCK_STREAM,0); 
    printf("file descriptor 4:%d\n",fd4);
    close(fd1);
    close(fd3);
    close(fd4);
    return 0;
}
/*
输出
file descriptor 1:6
file descriptor 2:7
file descriptor 3:8
file descriptor 4:7

感觉文件描述符就是从头分配到尾，中途有人释放，有可能被重新占用

*/