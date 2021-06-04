# 测试多播
### 文件
- news_receiver.c
- news_sender.c
- TestA.c
- TestB.c

### 用法
先运行receiver,再运行sender


### 作用
news_sender 会把news.txt的内容，发送到多播组，而news_receiver会加入到多播组。但是news_reveiver的套接字并没有设置地址重用，所以在bind了某个地址及端口后，这个地址端口不能再被其他程序用了。所以问题就是，我不能在一台机器模拟出一个多播组上的多台机器。(但是不用担心，这些后来都被解决了)

TestA.c和TestB.c主要测试了puts和fputs的不同点。

