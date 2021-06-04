#include <stdio.h>
int main(void)
{
    int clnt[3];
    printf("sizeof(clnt) == %d\n", sizeof(clnt));
    return 0;
}
/* 
输出
12

说明sizeof() 可以直接知道数组的大小
 */