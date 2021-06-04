# 文件描述符与FILE转换
> 本质上是无缓冲的IO与有缓冲的IO

### 文件
- dup.c
- sep_clnt.c
- sep_serv.c

### 作用
dup.c 中，主要展示了dup和dup2两个函数的作用。sep_serv.c中，用fdopen()以及一个文件描述符生成了FILE * fread 和FILE * fread。先是sep_serv向sep_clnt发送，sep_serv执行shutdown之后，sep_clnt会跳出循环，反过来给sep_serv发信息。sep_serv也还是可以收到的。

### 相关函数
- dup(int fd);
- dup(int fd,int tar_fd);
- fdopen(int fd,char * model);
- fileno(FILE * fp);