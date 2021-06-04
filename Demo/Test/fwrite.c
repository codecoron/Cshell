#include <stdio.h>

int main(void)
{
    FILE *fr;
    FILE *fw;

    fr = fopen("read.txt", "rb");
    fw = fopen("write.txt", "wb");

    return 0;
}
/*
测试rb 和wb
*/