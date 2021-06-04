#include <stdio.h>
#include <string.h>
int main(void)
{
    char buf[1024];
    fgets(buf, sizeof(buf), stdin);
    printf("%s", buf);
    printf("-------------------\n");
    printf("sizeof(buf) == %d\n", sizeof(buf));
    printf("strlen(buf)==%d\n", strlen(buf));
    return 0;
}
/* 
输出
hello world
-------------------
sizeof(buf) == 1024
strlen(buf)==12

结果:strlen() 包括 空格和斜杠
 */