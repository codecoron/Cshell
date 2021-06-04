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
#define NameSize 32

//服务器信息
int serv_sock;
struct sockaddr_in serv_adr;

//用于select的集合
fd_set reads;
fd_set cpy_reads;
int fd_max;

//缓存定义
char Recvbuf[buf_SIZE];
char Name[NameSize]; //名字
char Inputbuf[buf_SIZE / 2];
char Filename[NameSize];
char stageInput[buf_SIZE];
int still_input_flag = -1;

int telnet_sock = -1;

void error_handling(char *message);
void Shell();
void Read_and_send(char *filename);
void Excute();
void ReturnFile(char *filename);
void RecvFile(char *filename);

int main(int argc, char *argv[])
{
    if (argc < 4)
        error_handling("Usage <ServIP> <Port> <Name>\n");

    serv_sock = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    memset(&serv_adr, 0, sizeof(serv_adr));
    serv_adr.sin_family = AF_INET;
    serv_adr.sin_addr.s_addr = inet_addr(argv[1]);
    // serv_adr.sin_addr.s_addr = inet_addr("100.2.169.8");
    serv_adr.sin_port = htons(atoi(argv[2]));

    if (connect(serv_sock, (struct sockaddr *)&serv_adr, sizeof(serv_adr)) == -1)
        error_handling("connect() error\n");

    strcpy(Name, argv[3]);
    printf("Name:%s\n", Name);
    printf("Connect Ok ServerIp %s \t%d\n", inet_ntoa(serv_adr.sin_addr), ntohs(serv_adr.sin_port));

    char OnlineMsg[40];
    sprintf(OnlineMsg, "[%s] %s\n", Name, "Join");
    write(serv_sock, OnlineMsg, sizeof(OnlineMsg));

    FD_ZERO(&reads);
    fd_max = serv_sock;
    FD_SET(0, &reads);
    FD_SET(serv_sock, &reads);

    while (1)
    {
        cpy_reads = reads;
        int ret = select(fd_max + 1, &cpy_reads, NULL, NULL, NULL);

        if (ret == -1)
            error_handling("select() error\n");
        else
            Shell();
    }

    return 0;
}

void Shell()
{
    if (FD_ISSET(0, &cpy_reads)) //从键盘读取数据
    {
        memset(Recvbuf, 0, buf_SIZE);
        read(0, Recvbuf, buf_SIZE);

        char Tempbuf[buf_SIZE];
        char telnet_char[32];
        memcpy(Tempbuf, Recvbuf, sizeof(Recvbuf));
        char *p = strtok(Tempbuf, " ");
        if (p != NULL)
        {
            char *pnext = strtok(NULL, " ");
            if (pnext != NULL)
            {
                memcpy(telnet_char, p + 1, pnext - p);
            }
        }
        if (0 == strcmp(telnet_char, "telnet"))
        {
            printf("Telnet Open\n");
            telnet_sock = atoi(p);
        }

        if (Recvbuf[0] == ':' || Recvbuf[0] == '$')
            write(serv_sock, Recvbuf, strlen(Recvbuf));
        else
        {
            char Messagebuf[buf_SIZE + NameSize + 2];
            memset(Messagebuf, 0, sizeof(Messagebuf));
            sprintf(Messagebuf, "[%s]%s", Name, Recvbuf);
            write(serv_sock, Messagebuf, strlen(Messagebuf));
        }
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

        //对字符串进行分解，分解之后，可以知道是「命令」还是「信息」
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

        char cmd[32];
        if (p != NULL)
            sprintf(cmd, "%s", p);

        if (Recvbuf[0] == '[') ////默认为聊天
        {
            // fgets(stageInput, 1, stdin);
            // still_input_flag = scanf("%s", stageInput);
            // fflush(stdin);
            // still_input_flag = read(0, stageInput, buf_SIZE);
            write(1, Recvbuf, sizeof(Recvbuf));
        }
        else if (0 == strcmp(cmd, "fetch"))
        {
            p = strtok(NULL, " ");
            if (p != NULL)
            {
                sprintf(Filename, "%s", p);
                //Filename[strlen(Filename) - 1] = '\0';
                printf("Return fetch filename :%s\n", Filename);
                ReturnFile(Filename);
            }
        }
        else if (0 == strcmp(cmd, "push"))
        {
            p = strtok(NULL, " ");
            if (p != NULL)
            {
                sprintf(Filename, "%s", p);
                //Filename[strlen(Filename) - 1] = '\0';
                printf("push filename :%s\n", Filename);
                ReturnFile(Filename);
            }
        }
        else if (0 == strcmp(cmd, "return"))
        {
            p = strtok(NULL, " ");
            if (p != NULL)
            {
                sprintf(Filename, "%s", p);
                //接受文件
                char create_cmd[64];
                sprintf(create_cmd, "%s %s", "touch", Filename);
                RecvFile(Filename);
                write(1, "成功fetch文件\n", strlen("成功fetch文件\n"));
            }
        }
        else if (Recvbuf[0] == 'e')
            Excute();
        else if (Recvbuf[0] != '[')
            write(1, Recvbuf, sizeof(Recvbuf));

        // if (still_input_flag > 0)
        // {
        //     printf("接着输入\n");
        //     fputs(stageInput, stdout);
        // }
    }
}

void Excute()
{
    //system("echo \"\" > cmd.txt ");
    system("touch cmd.txt");
    puts(Recvbuf);
    int ret = system(Recvbuf);
    if (ret < 0)
        printf("system() error\n");
    //发送文件给Server
    Read_and_send("cmd.txt");
    system("cat cmd.txt");
}

void ReturnFile(char *filename)
{
    sleep(1);
    char name[32];
    if (filename != NULL)
        sprintf(name, "%s", filename);
    Read_and_send(filename);
}

void Read_and_send(char *p)
{
    char filename[32];
    sprintf(filename, "%s", p);

    FILE *fp;
    int totlen;
    char Recvbuf[buf_SIZE];

    if ((fp = fopen(filename, "rb")) == NULL)
    {
        puts("fopen() error\n");
        return;
    }
    fseek(fp, 0, SEEK_END);
    totlen = ftell(fp);
    printf("Debug Send FileSize %d\n", totlen);
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

void RecvFile(char *p)
{
    char filename[32];
    if (p != NULL)
        sprintf(filename, "%s", p);

    char recvBuf[buf_SIZE];
    int ret = read(serv_sock, recvBuf, buf_SIZE);
    int Total = atoi(recvBuf); //先接收，文件大小
    printf("Deubug Recv FileSize %d\n", Total);
    FILE *fp;
    fp = fopen(filename, "wb");
    if (fp == NULL)
    {
        puts("fopen() error\n");
        return;
    }

    int cnt = 0;
    sleep(0.5);
    while (1)
    {
        memset(recvBuf, 0, sizeof(recvBuf));
        ret = read(serv_sock, recvBuf, SIZE);
        fwrite(recvBuf, ret, 1, fp);
        // if (recvBuf[0] == '\0')
        // {
        //     printf("Debug recvBuf[0] == 0\n");
        //     printf("Debug ret == %d\n", ret);
        // }
        // else
        // {
        //     printf("recvBuf[0] = %c\n", recvBuf[0]);
        // }
        cnt += ret;
        if (cnt >= Total || ret == -1)
            break;
    }
    fclose(fp);
    puts("End of Recv");
}

void error_handling(char *message)
{
    printf("%s", message);
    exit(0);
}
