#include<stdio.h>
#include<pthread.h>
#include<semaphore.h>

void * read(void *arg);
void * accu(void *arg);
static sem_t sem_one;
static sem_t sem_two;
static int num;

int main(int argc,char *argv[]){
	pthread_t id_t1;
	pthread_t id_t2;
	sem_init(&sem_one,0,0);
	sem_init(&sem_two,0,1);

	pthread_create(&id_t1,NULL,read,NULL);
	pthread_create(&id_t2,NULL,accu,NULL);

	pthread_join(id_t1,NULL);
	pthread_join(id_t2,NULL);

	sem_destroy(&sem_one);
	sem_destroy(&sem_two);
	return 0;
}

void * read(void *arg){
	int i;
	for(i=0;i<5;i++){
		fputs("Input num: ",stdout);
		sem_wait(&sem_two);
		scanf("%d",&num);
		sem_post(&sem_one);
	}
	return NULL;
}

void *accu(void *arg){
	int sum=0;
	int i;
	for(i=0;i<5;i++){
		sem_wait(&sem_one);
		sum+=num;
		sem_post(&sem_two);
	}
	printf("Result:%d\n",sum);
	return NULL;
}

/*
输出：正确

用到了信号量
1、sem_t 信号量类型
2、信号量init、sem_wait、sem_post、destory
第一次感觉到操作系统的知识在代码上实现了。
*/
