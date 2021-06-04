#include<stdio.h>
#include<stdlib.h>
#include<string.h>
struct set{
int a;
int b;
char * string;
};

int main(void){
    struct set MysetA;
    MysetA.a = 10;
    MysetA.string = (char *)malloc(sizeof("hello world"));
    strcpy(MysetA.string,"Hello world");

    printf("%d\n",MysetA.a);
   puts(MysetA.string);

    struct set MysetB = MysetA;
   printf("%d\n",MysetB.a);
    puts(MysetA.string);

    return 0;
}
/*
输出
10
Hello world
10
Hello world

原来结构体可以直接赋值的
*/