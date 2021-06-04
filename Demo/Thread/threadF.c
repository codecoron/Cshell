#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <pthread.h>

#define NUM_THREAD 100
void *thread_inc(void *arg);
void *thread_des(void *arg);
long long sum = 0;
pthread_mutex_t mutex;

int main(int argc, char *argv[])
{
    pthread_t thread_id[NUM_THREAD];
    int i;
    printf("sizeof long long :%ld\n", sizeof(long long));
	
    pthread_mutex_init(&mutex,NULL);

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
    pthread_mutex_destroy(&mutex);
    return 0;
}
void *thread_inc(void *arg)
{
    int i;
    for (i = 0; i < 5; i++){
	pthread_mutex_lock(&mutex);
        sum += 1;
	printf("inc %d\n",i);
	pthread_mutex_unlock(&mutex);
    }
    return NULL;
}
void *thread_des(void *arg)
{
    int i;
    for (i = 0; i < 5; i++){
	pthread_mutex_lock(&mutex);
        sum -= 1;
	printf("dec %d\n",i);
	pthread_mutex_unlock(&mutex);
    }
    return NULL;
}

/*
输出:
最后的结果是0
结果正确

应该掌握
1、互斥量的类型 pthread_mutex_t
2、互斥量的初始化，lock,unlock、destory
*/
