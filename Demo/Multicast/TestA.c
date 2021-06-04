#include<stdio.h>
#include<string.h>
int main(void){

    char buf[30] ;
    memset(buf,0,sizeof(buf));
    buf[0] = 'a';
    buf[1] = 'b';
    buf[2] = 'c';
    // buf[3] = '\n';
    fputs(buf,stdout);

    while (1)
    {
        /* code */
    }
    

    return 0;
}

/*
1、fputs只有在遇到\n或者感觉到程序运行结束了，才会输出。fputs可以指定流方向
2、puts会默认补上\n ,默认流方向stdout
*/