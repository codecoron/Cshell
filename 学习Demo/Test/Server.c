#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>

void error_handling(char *message);

int main(int argc, char *argv[])
{
    int serv_sock;                               //服务器socket
    struct sockaddr_in serv_adr;                 //服务器地址
    serv_sock = socket(AF_INET, SOCK_STREAM, 0); //IPPORT_TELNET
    memset(&serv_adr, 0, sizeof(serv_adr));
    serv_adr.sin_family = AF_INET;
    serv_adr.sin_addr.s_addr = inet_addr("100.2.169.8");
    serv_adr.sin_port = htons(9190);

    if (bind(serv_sock, (struct sockaddr *)&serv_adr, sizeof(serv_adr)) == -1)
        error_handling("bind() error\n");
    printf("bind Ok ServerIp %s \t%d\n", inet_ntoa(serv_adr.sin_addr), ntohs(serv_adr.sin_port));
    if (listen(serv_sock, 1) == -1)
        error_handling("listen() error\n");

    int tongxunsock = accept(serv_sock, NULL, 0);
    if (tongxunsock == -1)
        error_handling("accept() error\n");

    char buf[8];
    int len = recv(tongxunsock, buf, 8, 0);

    printf("len = %d\n", len);
    printf("%s\n", buf);
    while (1)
    {
        /* code */
        int len = recv(tongxunsock, buf, 8, 0);

        printf("len = %d\n", len);
        printf("%s\n", buf);
    }

    return 0;
}

void error_handling(char *message)
{
    printf("%s", message);
    exit(0);
}