# CShell 1.0
> 1.0版本用基本的TCP模仿出了群聊功能

### 文件
- telcnt.c
- Client.c
- Server.c

telcnt.c是Client.c的前身，Client.c在telcnt.c的基础上，增加了Select模型。

### 作用
Client.c使用Select模型处理键盘输入和服务端发过来的信息。Server.c用Select处理客户端的连接请求和客户端发送的消息。

### 需要关注的技术点
- Server会同时处理很多TCP连接，正好发挥了Select的作用。