#include <stdio.h>
int main(void)
{
    enum Command
    {
        ls = 1
    };

    enum Command com;
    char *p = "ls";
    com = p;

    switch (com)
    {
    case ls:
        printf("switch %s\n", com);
        break;

    default:
        break;
    }
    return 0;
}
/*
switch支持数字，和枚举类型。不支持字符串
*/