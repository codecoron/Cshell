#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <sys/select.h>

#define Max_clnt_cnt 100
#define BUF_SIZE 1024

int clnt_cnt = 0;
socklen_t socklen_IPV4;
int clnt_socks[Max_clnt_cnt];
struct sockaddr_in clnt_adrs[Max_clnt_cnt];

void error_handling(char *message);

int main(int argc, char *argv[])
{
    int serv_sock;               //服务器socket
    struct sockaddr_in serv_adr; //服务器地址

    serv_sock = socket(AF_INET, SOCK_STREAM, 0); //IPPORT_TELNET
    memset(&serv_adr, 0, sizeof(serv_adr));
    serv_adr.sin_family = AF_INET;
    //serv_adr.sin_addr.s_addr = htonl(INADDR_ANY);
    //char ip[20];
    //puts("Enter ServerIp:");
    //scanf("%s", ip);
    serv_adr.sin_addr.s_addr = inet_addr("100.2.169.8");
    //serv_adr.sin_addr.s_addr = inet_addr(ip);
    serv_adr.sin_port = htons(9190);

    if (bind(serv_sock, (struct sockaddr *)&serv_adr, sizeof(serv_adr)) == -1)
        error_handling("bind() error\n");
    printf("bind Ok ServerIp %s \t%d\n", inet_ntoa(serv_adr.sin_addr), ntohs(serv_adr.sin_port));
    if (listen(serv_sock, Max_clnt_cnt) == -1)
        error_handling("listen() error\n");

    //accept(serv_sock, NULL, 0);

    fd_set reads;
    fd_set cpy_reads;
    int fd_max;
    FD_ZERO(&reads);
    fd_max = serv_sock;
    FD_SET(serv_sock, &reads);

    while (1)
    {
        /* SELECT 处理连接 */
        cpy_reads = reads;
        struct timeval time_out;
        time_out.tv_sec = 1;
        time_out.tv_usec = 0;
        int ret = select(fd_max + 1, &cpy_reads, NULL, NULL, &time_out);
        if (ret == -1)
            error_handling("select() error\n");
        else if (ret == 0)
            continue;
        else
        {
            for (int i = 0; i < fd_max + 1; i++)
            {
                if (FD_ISSET(i, &cpy_reads))
                {
                    if (i == serv_sock)
                    {
                        struct sockaddr_in tmp_cln_adr;
                        int tongxunsock = accept(serv_sock, (struct sockaddr *)&tmp_cln_adr, &socklen_IPV4);
                        clnt_adrs[++clnt_cnt] = tmp_cln_adr;
                        FD_SET(tongxunsock, &reads);
                        if (fd_max < tongxunsock)
                            fd_max = tongxunsock;
                        printf("connected client :%d\t %s %d\n", tongxunsock, inet_ntoa(tmp_cln_adr.sin_addr), ntohs(tmp_cln_adr.sin_port));
                    }
                    else
                    {
                        sleep(1); //睡1秒确保一次性收取所有信息
                        char buf[BUF_SIZE];
                        int str_len;
                        memset(buf, 0, BUF_SIZE);
                        str_len = read(i, buf, BUF_SIZE); //TODO 数据太长，要接受完再发送
                        if (str_len == 0)
                        {
                            printf("client:%d 断开连接\n", i);
                            FD_CLR(i, &reads);
                            close(i);
                        }
                        else
                        {
                            for (int j = serv_sock + 1; j < fd_max + 1; j++)
                            {
                                if (j != i)
                                {
                                    write(j, buf, strlen(buf));
                                    printf("返回数据给client:%d-->%s", j, buf);
                                }
                            }
                        }
                    }
                }
            }
        }
    }
    close(serv_sock);

    return 0;
}

void error_handling(char *message)
{
    printf("%s", message);
    exit(0);
}