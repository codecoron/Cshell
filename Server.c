#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <sys/select.h>
#include <sys/time.h>
#define Max_clnt_cnt 100
#define BUF_SIZE 1024
#define SIZE 1024

int clnt_cnt = 0; //客户端连接数
socklen_t socklen_IPV4;
int clnt_socks[Max_clnt_cnt];               //客户端的socket数组
struct sockaddr_in clnt_adrs[Max_clnt_cnt]; //客户端socket数组对应的ip地址
int telnet_data[Max_clnt_cnt];

/*要连接的服务器信息*/
int serv_sock;               //服务器socket
struct sockaddr_in serv_adr; //服务器地址

/*用于Select的全局变量*/
fd_set reads;
fd_set cpy_reads;
int fd_max;

/*全局缓冲区*/
char buf[BUF_SIZE];

void error_handling(char *message);
void socket_handling();
void Execute(int sock, int cmd_len);
void Message_handle(int sock);
void Command_ls(int sock);
void Recv_a_Send(int sock, char *filename);

int main(int argc, char *argv[])
{

    serv_sock = socket(AF_INET, SOCK_STREAM, 0);
    memset(&serv_adr, 0, sizeof(serv_adr));
    serv_adr.sin_family = AF_INET;
    serv_adr.sin_addr.s_addr = inet_addr("100.2.169.8");
    serv_adr.sin_port = htons(atoi(argv[1]));
    //serv_adr.sin_port = htons(9991);

    if (bind(serv_sock, (struct sockaddr *)&serv_adr, sizeof(serv_adr)) == -1)
        error_handling("bind() error\n");
    printf("bind Ok ServerIp %s \t%d\n", inet_ntoa(serv_adr.sin_addr), ntohs(serv_adr.sin_port));
    if (listen(serv_sock, Max_clnt_cnt) == -1)
        error_handling("listen() error\n");

    FD_ZERO(&reads);
    fd_max = serv_sock;
    FD_SET(serv_sock, &reads);

    while (1)
    {
        /* SELECT 处理连接 */
        cpy_reads = reads;
        struct timeval time_out;
        time_out.tv_sec = 2;
        time_out.tv_usec = 0;
        int ret = select(fd_max + 1, &cpy_reads, NULL, NULL, &time_out);
        if (ret == -1)
            error_handling("select() error\n");
        else if (ret == 0)
            continue;
        else
            socket_handling();
    }
    close(serv_sock);

    return 0;
}

void socket_handling()
{
    for (int i = 0; i < fd_max + 1; i++)
    {
        if (FD_ISSET(i, &cpy_reads))
        {
            if (i == serv_sock)
            {
                struct sockaddr_in tmp_cln_adr;
                int tongxunsock = accept(serv_sock, (struct sockaddr *)&tmp_cln_adr, &socklen_IPV4);

                struct timeval Recvtimeout;
                Recvtimeout.tv_sec = 3;
                Recvtimeout.tv_usec = 0;
                setsockopt(tongxunsock, SOL_SOCKET, SO_RCVTIMEO, &Recvtimeout, sizeof(Recvtimeout));

                clnt_adrs[clnt_cnt++] = tmp_cln_adr;
                clnt_socks[clnt_cnt - 1] = tongxunsock;
                FD_SET(tongxunsock, &reads);
                if (fd_max < tongxunsock)
                    fd_max = tongxunsock;
                printf("connected client :%d\t %s %d\n", tongxunsock, inet_ntoa(tmp_cln_adr.sin_addr), ntohs(tmp_cln_adr.sin_port));
            }
            else
            {
                sleep(0.2); //睡1秒确保一次性收取所有信息
                int str_len;
                memset(buf, 0, BUF_SIZE);
                str_len = read(i, buf, BUF_SIZE);
                if (str_len == 0)
                {
                    printf("client:%d 断开连接\n", i);
                    FD_CLR(i, &reads);
                    close(i);
                    int location;
                    for (int k = 0; k < sizeof(clnt_socks); k++) //TODO 这里只是将某个数组上的位置置0，可以做到移动数组
                    {
                        if (clnt_socks[k] == i)
                        {
                            clnt_socks[k] = -1;
                            clnt_adrs[k].sin_addr.s_addr = 0;
                            clnt_adrs[k].sin_port = 0;
                        }
                    }
                }
                else //对客户端的数据进行处理
                {
                    printf("buf[0] = %c\n", buf[0]);
                    // char *p;
                    // p = strtok(buf, " ");
                    // char retcmd[32];
                    // if (p != NULL)
                    //     sprintf(retcmd, "%s", p);
                    if (buf[0] == ':') //执行客户端命令
                        Execute(i, str_len);
                    else if (buf[0] == '$') //执行服务端命令
                    {
                        if (buf[1] == 'l' && buf[2] == 's')
                            Command_ls(i); //处理  服务端「$ls」 命令
                    }
                    // else if (0 == strcmp(retcmd, "return"))
                    // { //TODO 服务端要把文件接受下来，再发送
                    //     char filename[32];
                    //     memset(filename, 0, sizeof(filename));
                    //     recv(i, filename, sizeof(filename), 0);
                    //     Recv_a_Send(i - 1, filename);
                    // }
                    else
                        Message_handle(i);
                }
            }
        }
    }
}

void Message_handle(int sock)
{
    for (int j = serv_sock + 1; j < fd_max + 1; j++)
    {
        if (j != sock)
        {
            write(j, buf, strlen(buf));
            printf("返回数据给client:%d-->%s", j, buf);
        }
    }
}

void Execute(int sock, int cmd_len)
{
    char prefix[] = "echo 8002904 | sudo -S";
    //char backfix[] = "> cmd.txt 2>&1";
    char backfix[] = "";
    char cmd_buf[BUF_SIZE + 40];
    char tmp[BUF_SIZE];
    memset(tmp, 0, sizeof(tmp));
    strncpy(tmp, buf + 1, strlen(buf));
    //tmp[str_len - 2] = '\0';
    tmp[cmd_len - 2] = '\0';
    printf("tmp == %s\n", tmp);
    sprintf(cmd_buf, "%s %s %s", prefix, tmp, backfix);
    puts(cmd_buf);

    write(sock + 1, cmd_buf, strlen(cmd_buf));
    //printf("return cmd result\n");
    Recv_a_Send(sock, "Temp.txt");
}

void Recv_a_Send(int sock, char *p)
{
    char filename[32];
    if (p != NULL)
        sprintf(filename, "%s", p);

    char recvBuf[BUF_SIZE];
    int ret = recv(sock + 1, recvBuf, BUF_SIZE, 0);
    int Total = atoi(recvBuf); //先接收，文件大小
    //接受
    FILE *fp;
    fp = fopen("Temp.txt", "wb");
    if (fp == NULL)
        error_handling("fopen() error\n");

    int cnt = 0;
    sleep(1);
    while (1)
    {
        memset(recvBuf, 0, sizeof(recvBuf));
        ret = recv(sock + 1, recvBuf, SIZE, 0);
        fwrite(recvBuf, ret, 1, fp);
        if (recvBuf[0] == '\0')
        {
            printf("recvBuf[0] == 0\n");
            printf("ret == %d\n", ret);
        }
        else
        {
            printf("recvBuf[0] = %c\n", recvBuf[0]);
        }
        write(sock, recvBuf, ret); //接完就发
        cnt += ret;
        if (cnt >= Total || ret == -1)
            break;
    }

    fclose(fp);
}

void Command_ls(int sock)
{
    for (int k = 0; k < clnt_cnt; k++)
    {
        char hostMess[50];
        strcpy(hostMess, inet_ntoa(clnt_adrs[k].sin_addr));
        char port[8];
        sprintf(port, "%d", ntohs(clnt_adrs[k].sin_port));
        strcat(hostMess, "\t");
        strcat(hostMess, port);
        strcat(hostMess, "\tsockNum\t");
        char sockNum[8];
        sprintf(sockNum, "%d", clnt_socks[k]);
        strcat(hostMess, sockNum);
        strcat(hostMess, "\n");
        send(sock, hostMess, strlen(hostMess), 0);
    }
}

void error_handling(char *message)
{
    printf("%s", message);
    exit(0);
}
