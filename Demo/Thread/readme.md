# 尝试使用Linux C的多线程

### 文件
- semaphore.c
- threadA.c
- threadB.c
- threadC.c
- threadD.c
- threadF.c

主要是测试线程的知识点，在每个文件的后面都有注释。

- chat_client.c
- chat_server.c

chat_client.c 与chat_server.c 组成了多人聊天室，server用多线程处理了多个客户端的连接和客户端的消息。client也用多线程处理接受屏幕键入信息和服务器发送的信息。


### 主要留下了这么几个知识点。
- pthread_t 类型 
- pthread_create 创建一个线程，每个参数是什么意思。主要的参数有三个，传出线程ID，传入回调函数，传入的参数。
- pthread_join 线程的加入。主要的参数有两个，传入线程ID，传出返回值。
- pthread_detach 与pthread_join的区别，detach不会阻塞主线程，join会阻塞主线程。