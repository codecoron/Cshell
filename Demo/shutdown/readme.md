# 基于TCP发送文件
> 重点使用了shutdown函数

### 文件
- file_client.c
- file_server.c

### 作用
file_server 把 file_server.txt的内容发送给file_client，file_client把接收到的存为receive.dat,然后file_server使用shutdown(),变成只能接收。

为什么需要用半关闭？这里的例子就挺明显的，file_server需要发送EOF,客户端才能跳出while循环。如果file_server调用的是close()，这时候file_server就不能接收file_client的消息了。而用shutdown()则可以继续接收。