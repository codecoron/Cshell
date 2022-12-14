#include <sys/types.h>
#include <sys/socket.h>
#include <stdio.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <string.h>
#include <stdlib.h>
#include <fcntl.h>
#include <sys/shm.h>
#include <iostream>
#include <thread>
#include<pthread.h>
#define PORT 7000
#define QUEUE 20
int ss;
struct sockaddr_in client_addr;
socklen_t length = sizeof(client_addr);
int conns[2] = {}; //定义了一个容量为2的数组来存放套接字，所以server最多只能跟2个client通信
int z = 0;
void thread_fn()
{
    //成功返回非负描述字，出错返回-1
    int conn = accept(ss, (struct sockaddr *)&client_addr, &length);
    if (conn < 0)
    {
        perror("connect");
        exit(1);
    }
    //把连接保存到临时数组中;
    conns[z] = conn;
    z++;

    fd_set rfds;
    struct timeval tv; //linux编程中，如果用到计时，可以用struct timeval获取系统时间
    int retval, maxfd;
    while (1)
    {
        /*把可读文件描述符的集合清空*/
        FD_ZERO(&rfds);
        /*把标准输入的文件描述符加入到集合中*/
        FD_SET(0, &rfds);
        maxfd = 0;
        /*把当前连接的文件描述符加入到集合中*/
        FD_SET(conn, &rfds);
        /*找出文件描述符集合中最大的文件描述符*/
        if (maxfd < conn)
        {
            maxfd = conn;
        }
        /*设置超时时间*/
        tv.tv_sec = 5; //5秒
        tv.tv_usec = 0;
        /*等待聊天*/
        retval = select(maxfd + 1, &rfds, NULL, NULL, &tv);
        if (retval == -1)
        {
            printf("select出错，客户端程序退出\n");
            break;
        }
        else if (retval == 0)
        {
            printf("服务端没有任何输入信息，并且客户端也没有信息到来，waiting...\n");
            continue;
        }
        else
        {
            /*客户端发来了消息*/
            if (FD_ISSET(conn, &rfds)) //判断conn是否在rfds中如果在返回非零，不再返回0
            {
                char buffer[1024];
                memset(buffer, 0, sizeof(buffer));               //把buffer中的所有值赋值为0，即清空buffer
                int len = recv(conn, buffer, sizeof(buffer), 0); //把接收到的数据存放于buffer中
                if (strcmp(buffer, "exit\n") == 0)               //如果接受到的是空的，即没有收到任何信息
                    break;
                printf("%s", buffer);
                //send(conn, buffer, len , 0);把数据回发给客户端
            }
            /*用户输入信息了,开始处理信息并发送*/
            if (FD_ISSET(0, &rfds))
            {
                char buf[1024];
                fgets(buf, sizeof(buf), stdin); //每次读取一行数据存放在buf中
                //printf("you are send %s", buf);
                for (int i = 0; i < z; i++)
                {
                    send(conns[i], buf, sizeof(buf), 0);
                }
            }
        }
    }
    close(conn);
}
void thread_select(int conn)
{
}
int main()
{
    ss = socket(AF_INET, SOCK_STREAM, 0); //SOCK_STREAM即tcp协议，AF_INET是IPv4套接字
    struct sockaddr_in server_sockaddr;
    server_sockaddr.sin_family = AF_INET;
    server_sockaddr.sin_port = htons(PORT);
    //printf("%d\n",INADDR_ANY);
    server_sockaddr.sin_addr.s_addr = htonl(INADDR_ANY);
    if (bind(ss, (struct sockaddr *)&server_sockaddr, sizeof(server_sockaddr)) == -1)
    {
        perror("bind");
        exit(1);
    }
    if (listen(ss, QUEUE) == -1)
    {
        perror("listen");
        exit(1);
    }
    std::thread t(thread_fn);  //因为创建了两个线程所以只能连接两个client
    std::thread t1(thread_fn); //这里把收发数据都存放在thread_fn中，所以创建一个这样的线程就能使得server能多连接一个server
    t.join();
    t1.join();
    close(ss);
    return 0;
}