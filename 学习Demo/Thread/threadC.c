#include<stdio.h>
#include<pthread.h>

void * thread_sum(void *arg);
int sum = 0;

int main(int argc,char *argv[]){
	pthread_t id_t1;
	pthread_t id_t2;
	int range1[] = {1,5};
	int range2[] = {6,10};
	pthread_create(&id_t1,NULL,thread_sum,(void *)range1);
	pthread_create(&id_t2,NULL,thread_sum,(void *)range2);

	pthread_join(id_t1,NULL);
	pthread_join(id_t2,NULL);
	printf("result:%d\n",sum);
	return 0;
}

void * thread_sum(void * arg){
	int start = ((int *)arg)[0];
	int end = ((int *)arg)[1];
	while(start<=end){
		sum+=start;
		start++;
	}
	return NULL;
}

/*
输出:
result:55

这个结果是正确的。但是存在一个互斥资源的访问问题。
因为create的线程之间会来回切换。

*/