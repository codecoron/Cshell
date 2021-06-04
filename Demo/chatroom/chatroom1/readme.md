# 聊天室1.0
> 1.0版本是基于多播和多进程
### 文件
- selectserv.c
- client.c

### 作用
client 会与Server建立TCP连接，同时也为UDP套接字绑定一个地址，并且将这个UDP套接字加入多播组。Server接受client的TCP连接，然后将收到的信息，转发到多播组。

但是在传输字符串的时候，有错误，没有处理好
### 需要关注的一些细节
1. 多播地址，有一个结构体来存储struct ip_mreq
2. 创建子进程的fork()，以及fork()的返回值
3. 通过setsockopt设置bind的地址的重用SO_REUSEADDR
4. 通过setsockopt将UDP加入多播组