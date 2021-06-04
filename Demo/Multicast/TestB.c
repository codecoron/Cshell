#include<stdio.h>
int main(void){
    // char buf[30] = {0,};
    // puts(buf);

    // puts("hello\n");//程序结束前会有输出
    fputs("hello",stdout);//程序结束前，没有输出
    fflush(stdout);//fflush配上fputs就可以马上有输出
    while (1)
    {
        /* code */
    }
    
    return 0;
}
/*
1、puts会自动刷新缓冲区
2、fflush不会自动自动刷新缓冲区
*/