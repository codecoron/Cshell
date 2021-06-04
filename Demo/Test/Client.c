#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>

void error_handling(char *message);

int main(void)
{
    int serv_sock;
    struct sockaddr_in serv_adr;
    serv_sock = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    memset(&serv_adr, 0, sizeof(serv_adr));
    serv_adr.sin_family = AF_INET;
    serv_adr.sin_addr.s_addr = inet_addr("100.2.169.8");
    serv_adr.sin_port = htons(9190);

    if (connect(serv_sock, (struct sockaddr *)&serv_adr, sizeof(serv_adr)) == -1)
        error_handling("connect() error\n");

    char buf[8] = "Hello";

    while (1)
    {
        /* code */
        int len = send(serv_sock, buf, 4, 0);
    }

    return 0;
}

void error_handling(char *message)
{
    printf("%s", message);
    exit(0);
}
/* 本来想测试 
Send 5个字节，
Recv 8个字节 ，
Recv函数会不会阻塞*/
