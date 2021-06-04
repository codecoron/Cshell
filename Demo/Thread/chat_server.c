#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <pthread.h>

#define BUF_SIZE 100
#define MAX_CLNT 256

void *handle_clnt(void *arg);
void send_msg(char *msg, int len);
void error_handling(char *msg);

int clnt_cnt = 0;
int clnt_socks[MAX_CLNT];
pthread_mutex_t mutex;

int main(int argc, char *argv[])
{
        int serv_sock;
        struct sockaddr_in serv_adr;
        // if (argc != 3)
        // {
        //      printf("Usage:%s <Server IP> <port>\n", argv[0]);
        //      exit(1);
        // }

        pthread_mutex_init(&mutex, NULL);
        serv_sock = socket(PF_INET, SOCK_STREAM, 0);

        memset(&serv_adr, 0, sizeof(serv_adr));
        serv_adr.sin_family = AF_INET;
        serv_adr.sin_addr.s_addr = inet_addr("192.168.187.146");
        //serv_adr.sin_addr.s_addr = htonl(INADDR_ANY);
        //serv_adr.sin_port = htons(atoi(argv[1]));
        serv_adr.sin_port = htons(9190);

        if (bind(serv_sock, (struct sockaddr *)&serv_adr, sizeof(serv_adr))==-1)
                error_handling("bind() error\n");


        if (listen(serv_sock, 100) == -1)
                error_handling("listen() error\n");

        while (1)
        {
                struct sockaddr_in clnt_adr;
                socklen_t s_len;
                int tongxunsock = accept(serv_sock, (struct sockaddr *)&clnt_adr, &s_len);
                if (tongxunsock == -1)
                        error_handling("accept() error\n");
                else 
                        printf("tongxunsock = %d\n",tongxunsock);
                pthread_mutex_lock(&mutex);
                clnt_socks[clnt_cnt++] = tongxunsock;
                pthread_mutex_unlock(&mutex);

                pthread_t t_id;
                pthread_create(&t_id, NULL, handle_clnt, (void *)&tongxunsock);
                pthread_detach(t_id);
                printf("Connected client IP:%s\n", inet_ntoa(clnt_adr.sin_addr));
        }

        close(serv_sock);
        return 0;
}

void error_handling(char *msg)
{
        puts(msg);
        exit(1);
}

void *handle_clnt(void *arg)
{
        int clnt_sock = *((int *)arg);
        int str_len = 0;
        int i;
        char msg[BUF_SIZE];
		        while ((str_len = read(clnt_sock, msg, sizeof(msg)) != 0))
        {
                fputs(msg,stdout);
                send_msg(msg, str_len);
        }

        pthread_mutex_lock(&mutex);
        for (i = 0; i < clnt_cnt; i++)
        {
                if (clnt_sock == clnt_socks[i])
                {
                        while (i++ < clnt_cnt - 1)
                                clnt_socks[i] = clnt_socks[i + 1];
                        break;
                }
        }
        clnt_cnt--;
        pthread_mutex_unlock(&mutex);
        close(clnt_sock);
        return NULL;
}

void send_msg(char *msg, int len)
{
        int i;
        pthread_mutex_lock(&mutex);
        for (i = 0; i < clnt_cnt; i++){
                printf("clnt_socks[%d] == %d\n",i,clnt_socks[i]);
                write(clnt_socks[i], msg, len);
        }
        pthread_mutex_unlock(&mutex);
}

//我在循环里面定义变量，会造成内存的浪费吗？
                                                       