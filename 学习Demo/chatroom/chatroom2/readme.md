# 聊天室2.0
> 其实算不上2.0，但是已经放弃了多播，Client还有多播的代码，Server暂时改成回声

### 文件
- client.c
- echo_selectserv.c

### 作用
client用上了select,echo_selectserv改成回声服务器，就是谁发消息，我就返回给谁。但是客户端还没有做接收TCP的处理。所以这是一个不完整，不能完全运行的程序。