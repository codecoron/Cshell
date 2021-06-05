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
#define NameSize 32
#define Passwdsize 32

int clnt_cnt = 0; //客户端连接数
socklen_t socklen_IPV4;
int clnt_socks[Max_clnt_cnt];               //客户端的socket数组
struct sockaddr_in clnt_adrs[Max_clnt_cnt]; //客户端socket数组对应的ip地址
char clnt_Name[Max_clnt_cnt][NameSize];     //名字数组
int telnet_sock = -1;                       //远程连接的socket号
int self_sock = -1;                         //主动telnet别人的socket号
char password[Max_clnt_cnt][20];            //存储telnet时的密码
int telnet_socks[Max_clnt_cnt];
/*要连接的服务器信息*/
int serv_sock;               //服务器socket
struct sockaddr_in serv_adr; //服务器地址

/*用于Select的全局变量*/
fd_set reads;
fd_set cpy_reads;
int fd_max;

/*全局缓冲区*/
char buf[BUF_SIZE];
char Filename[NameSize]; //存储Fetch的文件名

void error_handling(char *message);
void socket_handling();
void Execute(int sock, int cmd_len);
void Message_handle(int sock);
void Server_ls(int sock);
void Recv_and_Send(int sock, char *filename);
void Fetch(int mysock, int target_sock, int cmd_len);
void Sendfile(int sock, char *p);
char *command_cat(int cmd_len, int sock);

int main(int argc, char *argv[])
{

    if (argc < 3)
        error_handling("Usage <IP> <Port>\n");

    serv_sock = socket(AF_INET, SOCK_STREAM, 0);
    memset(&serv_adr, 0, sizeof(serv_adr));
    serv_adr.sin_family = AF_INET;
    serv_adr.sin_addr.s_addr = inet_addr(argv[1]);
    //serv_adr.sin_addr.s_addr = inet_addr("100.2.169.8");
    serv_adr.sin_port = htons(atoi(argv[2]));
    //serv_adr.sin_port = htons(9991);

    if (bind(serv_sock, (struct sockaddr *)&serv_adr, sizeof(serv_adr)) == -1)
        error_handling("bind() error\n");
    printf("bind Ok ServerIp %s \t%d\n", inet_ntoa(serv_adr.sin_addr), ntohs(serv_adr.sin_port));
    if (listen(serv_sock, Max_clnt_cnt) == -1)
        error_handling("listen() error\n");

    FD_ZERO(&reads);
    fd_max = serv_sock;
    FD_SET(serv_sock, &reads);

    /*Init*/
    memset(telnet_socks, -1, sizeof(telnet_socks));

    while (1)
    {
        /* SELECT 处理连接 */
        cpy_reads = reads;
        int ret = select(fd_max + 1, &cpy_reads, NULL, NULL, NULL);
        if (ret == -1)
            error_handling("select() error\n");
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
            if (i == serv_sock) //有新的连接请求
            {
                struct sockaddr_in tmp_cln_adr;
                int tongxunsock = accept(serv_sock, (struct sockaddr *)&tmp_cln_adr, &socklen_IPV4);

                struct timeval Recvtimeout; //测试了很多次，给接受设置个超时比较好
                Recvtimeout.tv_sec = 3;
                Recvtimeout.tv_usec = 0;
                setsockopt(tongxunsock, SOL_SOCKET, SO_RCVTIMEO, &Recvtimeout, sizeof(Recvtimeout));
                clnt_cnt++;
                clnt_adrs[tongxunsock] = tmp_cln_adr;
                clnt_socks[tongxunsock] = tongxunsock;
                FD_SET(tongxunsock, &reads);
                if (fd_max < tongxunsock)
                    fd_max = tongxunsock;
                printf("connected client :%d\t %s %d\n", tongxunsock, inet_ntoa(tmp_cln_adr.sin_addr), ntohs(tmp_cln_adr.sin_port));
            }
            else
            {
                sleep(0.2); //睡0.2秒确保一次性收取所有信息
                int str_len;
                memset(buf, 0, BUF_SIZE);
                str_len = read(i, buf, BUF_SIZE);
                if (str_len == 0) //有客户端断开连接
                {
                    printf("client:%d 断开连接\n", i);
                    FD_CLR(i, &reads);
                    close(i);
                    if (i == fd_max)
                        fd_max--;
                    clnt_cnt--;
                    int location;
                    for (int k = 0; k < sizeof(clnt_socks); k++) //TODO 这里只是将某个数组上的位置置0，可以做到移动数组
                    {
                        if (clnt_socks[k] == i)
                        {
                            clnt_socks[k] = -1;
                            clnt_adrs[k].sin_addr.s_addr = 0;
                            clnt_adrs[k].sin_port = 0;
                            memset(clnt_Name[i], 0, sizeof(clnt_Name[i]));
                        }
                    }
                }
                else //对客户端的数据进行处理
                {
                    printf("Debug buf[0] = %c\n", buf[0]);

                    char temp[BUF_SIZE];
                    memcpy(temp, buf, sizeof(buf));
                    char *p;
                    p = strtok(temp, " ");
                    char cmd[32];
                    if (p != NULL)
                    {
                        sprintf(cmd, "%s", p + 1);
                        // printf("Debug cmd :%s\n", cmd);
                    }

                    if (0 == strcmp(buf, "$ls\n")) //处理  服务端「$ls」 命令
                        Server_ls(i);
                    else if (buf[0] == ':' && 0 == strcmp(cmd, "telnet"))
                    {
                        p = strtok(NULL, " ");
                        if (p != NULL)
                        {
                            telnet_socks[i] = atoi(p);
                            telnet_sock = atoi(p);
                        }
                        self_sock = i;

                        p = strtok(NULL, " ");
                        if (p != NULL)
                        {
                            memset(password[telnet_socks[i]], 0, sizeof(password[telnet_socks[i]]));
                            strcpy(password[telnet_socks[i]], p);
                            password[telnet_socks[i]][strlen(p) - 1] = '\0';
                        }
                        strcpy(password[i], "DefaultPassword"); //这是一个巨大的BUG
                        printf("Client [%d] want to telnet  Client [%d] password %s\n", i, telnet_socks[i], password[telnet_socks[i]]);
                    }
                    else if (buf[0] == ':' && 0 == strcmp(cmd, "fetch"))
                    {
                        p = strtok(NULL, " ");
                        if (p != NULL)
                        {
                            sprintf(Filename, "%s", p);
                            Filename[strlen(Filename) - 1] = '\0';
                        }
                        printf("Client [%d] want to Fetch filename :%s From Client [%d]\n", i, Filename, telnet_socks[i]);
                        Fetch(i, telnet_socks[i], str_len - 1);
                    }
                    else if (buf[0] == ':' && 0 == strcmp(cmd, "push"))
                    {
                        p = strtok(NULL, " ");
                        if (p != NULL)
                        {
                            sprintf(Filename, "%s", p);
                            Filename[strlen(Filename) - 1] = '\0';
                        }

                        printf("Client [%d] push filename :%s to Client [%d]\n", i, Filename, telnet_socks[i]);
                        Fetch(telnet_socks[i], i, str_len - 1);
                    }
                    else if (buf[0] == ':') //执行客户端命令
                        Execute(i, str_len - 1);
                    else
                        Message_handle(i);
                }
            }
        }
    }
}

void Message_handle(int sock)
{
    char temp[BUF_SIZE];
    memcpy(temp, buf, sizeof(buf));
    char *p;
    char Name[32];
    p = strtok(temp, "]");
    if (p != NULL)
    {
        char *pre = p;
        p = strtok(NULL, "]");
        if (p != NULL)
            memcpy(Name, pre + 1, p - pre - 1);
    }
    printf("Name:%s\n", Name);
    memcpy(clnt_Name[sock], Name, strlen(Name));
    printf("Clnt_Name[%d]:%s\n", sock, clnt_Name[sock]);

    for (int j = serv_sock + 1; j < fd_max + 1; j++)
    {
        if (j != sock)
        {
            write(j, buf, strlen(buf));
            buf[strlen(buf) - 1] = '\0';
            printf("返回数据 %s --> client:%d %s\n", buf, j, clnt_Name[j]);
        }
    }
}

char *command_cat(int cmd_len, int sock)
{
    char prefix[50];
    sprintf(prefix, "%s %s %s", "echo", password[sock], "| sudo -S");
    char backfix[] = " 2>&1";
    // char cmd_buf[BUF_SIZE + 40];
    char *cmd_buf = (char *)malloc(BUF_SIZE + 40);
    char tmp[BUF_SIZE];
    memset(tmp, 0, sizeof(tmp));
    strncpy(tmp, buf + 1, strlen(buf) - 1);
    tmp[cmd_len - 1] = '\0';
    //printf("Debug tmp == %s\n", tmp);
    sprintf(cmd_buf, "%s %s %s", prefix, tmp, backfix);
    puts(cmd_buf);
    return cmd_buf;
}

void Execute(int sock, int cmd_len)
{
    if (telnet_socks[sock] < 0)
    {
        printf("Debug sock:%d\t telneted:%d\n", sock, telnet_socks[sock]);
        printf("Debug telnet error\n");
        return;
    }

    char *p = command_cat(cmd_len, telnet_socks[sock]);
    char cmd_buf[BUF_SIZE + 40];
    memcpy(cmd_buf, p, BUF_SIZE + 40);

    write(telnet_socks[sock], cmd_buf, strlen(cmd_buf));
    int ret = system("touch Temp.txt");
    if (ret == -1)
    {
        puts("system(\"touch Temp.txt\") error");
        return;
    }
    Recv_and_Send(sock, "Temp.txt");
}

void Recv_and_Send(int sock, char *p)
{
    char filename[32];
    if (p != NULL)
        sprintf(filename, "%s", p);
    printf("Debug Recv and Send filename:%s\n", filename);

    char recvBuf[BUF_SIZE];
    int ret;
    if (telnet_socks[sock] > -1)
        ret = recv(telnet_socks[sock], recvBuf, BUF_SIZE, 0);
    int Total = atoi(recvBuf); //先接收，文件大小
    printf("Debug Recv FileSize %d\n", Total);
    FILE *fp;
    fp = fopen(filename, "wb");
    if (fp == NULL)
    {
        printf("foepn() error\n");
        return;
    }

    int cnt = 0;
    while (1)
    {
        memset(recvBuf, 0, sizeof(recvBuf));
        if (telnet_socks[sock] > -1)
            ret = read(telnet_socks[sock], recvBuf, SIZE);
        fwrite(recvBuf, ret, 1, fp);
        // if (Debug recvBuf[0] == '\0')
        // {
        //     printf("Debug recvBuf[0] == 0\n");
        //     printf("Debug ret == %d\n", ret);
        // }
        // else
        // {
        //     printf("Debug recvBuf[0] = %c\n", recvBuf[0]);
        // }
        write(sock, recvBuf, ret); //接完就发
        cnt += ret;
        if (cnt >= Total || ret == -1)
            break;
    }

    fclose(fp);
}

void Recvfile(int target_sock, char *p)
{
    char filename[32];
    if (p != NULL)
        sprintf(filename, "%s", p);

    printf("Debug Recv filename:%s\n", filename);

    char recvBuf[BUF_SIZE];
    int ret;
    ret = recv(target_sock, recvBuf, BUF_SIZE, 0);
    int Total = atoi(recvBuf); //先接收，文件大小
    printf("Debug Recv FileSize %d\n", Total);
    FILE *fp;
    fp = fopen(filename, "wb");
    if (fp == NULL)
        error_handling("fopen() error\n");

    int cnt = 0;
    sleep(1);
    while (1)
    {
        memset(recvBuf, 0, sizeof(recvBuf));
        ret = read(target_sock, recvBuf, SIZE);
        fwrite(recvBuf, ret, 1, fp);
        // if (recvBuf[0] == '\0')
        // {
        //     printf("Debug recvBuf[0] == 0\n");
        //     printf("Debug ret == %d\n", ret);
        // }
        // else
        // {
        //     printf("Debug recvBuf[0] = %c\n", recvBuf[0]);
        // }
        cnt += ret;
        if (cnt >= Total || ret == -1)
            break;
    }
    fclose(fp);
}

void Sendfile(int target_sock, char *p)
{
    char filename[32];
    sprintf(filename, "%s", p);

    char tmp_cmd[64] = "echo return  return return return return "; //发送命令是为了让客户端，知道这是给他的，让他准备接收
    char cmd[128];
    sprintf(cmd, "%s %s", tmp_cmd, filename);
    write(target_sock, cmd, strlen(cmd));
    write(1, cmd, strlen(cmd));
    sleep(1);

    FILE *fp;
    int totlen;
    char Recvbuf[BUF_SIZE];

    if ((fp = fopen(filename, "rb")) == NULL)
    {
        puts("fopen() error\n");
        return;
    }
    fseek(fp, 0, SEEK_END);
    totlen = ftell(fp);
    printf("Debug Send FileSize %d\n", totlen);
    //发送文件长度，方便客户端
    memset(Recvbuf, 0, sizeof(Recvbuf));
    sprintf(Recvbuf, "%d", totlen);
    write(target_sock, Recvbuf, strlen(Recvbuf) + 1);

    sleep(1);
    int cnt = totlen / SIZE;
    int len1 = SIZE;
    int lenlast;
    if (totlen % SIZE)
    {
        lenlast = totlen - cnt * SIZE;
        cnt = cnt + 1;
    }
    rewind(fp);

    for (int i = 0; i < cnt - 1; i++)
    {
        fread(Recvbuf, SIZE, 1, fp);
        sleep(0.1);
        send(target_sock, Recvbuf, SIZE, 0);
        memset(Recvbuf, 0, sizeof(Recvbuf));
    }
    fread(Recvbuf, lenlast, 1, fp);
    fclose(fp);
    send(target_sock, Recvbuf, lenlast, 0);
    printf("Send to [%d] OK\n", target_sock);
}

/* 
 从 target_socket 那里Fetch 文件 到 mysock
 cmd_len 是命令长度
 */

void Fetch(int mysock, int target_sock, int cmd_len)
{
    // if (telnet_socks[mysock] < 0)
    // {
    //     printf("Debug sock:%d\t telneted:%d\n", mysock, telnet_socks[mysock]);
    //     printf("telnet error\n");
    //     return;
    // }

    char *p = command_cat(cmd_len, target_sock);
    char cmd_buf[BUF_SIZE + 40];
    memcpy(cmd_buf, p, BUF_SIZE + 40);

    write(target_sock, cmd_buf, strlen(cmd_buf)); //发送命令

    char create_cmd[64];
    sprintf(create_cmd, "%s %s", "touch", Filename);
    int ret = system(create_cmd);
    if (ret == -1)
    {
        puts(create_cmd);
        puts("error");
        return;
    }

    //先发命令给  主动的客户端
    Recvfile(target_sock, Filename);
    sleep(2);
    Sendfile(mysock, Filename);
}

void Server_ls(int sock)
{
    for (int k = 0; k < fd_max + 1; k++)
    {
        if (clnt_socks[k] > 2)
        {
            char hostMess[50];
            char port[8];
            char sockNum[8];
            char Name[32];
            strcpy(hostMess, inet_ntoa(clnt_adrs[k].sin_addr));
            strcat(hostMess, "\t");
            sprintf(port, "%d", ntohs(clnt_adrs[k].sin_port));
            strcat(port, "\t");
            strcat(hostMess, port);
            strcat(hostMess, "sockNum\t");
            sprintf(sockNum, "%d", clnt_socks[k]);
            strcat(hostMess, sockNum);
            strcat(hostMess, "\t");
            strcat(hostMess, clnt_Name[k]);
            strcat(hostMess, "\n");
            send(sock, hostMess, strlen(hostMess), 0);
        }
    }
}

void error_handling(char *message)
{
    printf("%s", message);
    exit(0);
}
