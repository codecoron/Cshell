# 应用Select模型
### 文件
- echo_selectserv.c
- echo_selectclnt.c
- client.c
- Test.c

### 作用
echo_selectserv.c是服务端，用了select处理客户端的连接和通讯，将接收到的信息，原封不动地返回。client.c是客户端，只能从键盘读一段，然后发送一段。echo_selectclnt.c是用了select的客户端。可以解决阻塞在scanf的痛点。

Test.c则是测试了结构体之间能否用等号赋值。