# 简单UDP聊天
### 源文件
- uecho_client.c
- uecho_server.c

### 编译
gcc -std=c99 uecho_client.c -o client

gcc -std=c00 uecho_server.c -o server

### 效果
client发一条消息给server,server会把消息重新返回，用的是UDP发送。

### 函数
- sendto
- recvfrom