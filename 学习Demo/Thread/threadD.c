#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <pthread.h>

#define NUM_THREAD 100
void *thread_inc(void *arg);
void *thread_des(void *arg);
long long sum = 0;

int main(int argc, char *argv[])
{
    pthread_t thread_id[NUM_THREAD];
    int i;
    printf("sizeof long long :%d\n", sizeof(long long));

    for (i = 0; i < NUM_THREAD; i++)
    {
        if (i % 2)
            pthread_create(&(thread_id[i]), NULL, thread_inc, NULL);
        else
            pthread_create(&(thread_id[i]), NULL, thread_des, NULL);
    }

    for (i = 0; i < NUM_THREAD; i++)
        pthread_join(thread_id[i], NULL);

    printf("result:%lld\n", sum);
    return 0;
}
void *thread_inc(void *arg)
{
    int i;
    for (i = 0; i < 50000000; i++)
        sum += 1;
    return NULL;
}
void *thread_des(void *arg)
{
    int i;
    for (i = 0; i < 50000000; i++)
        sum -= 1;
    return NULL;
}
//输出结果，不固定
/*

创建了很多线程，一类用来加，一类用来减。但是新线程之间会来回切换
对本应互斥访问的资源，没有互斥访问。
*/
