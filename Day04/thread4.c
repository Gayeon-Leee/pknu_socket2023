#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <pthread.h>
#define NUM_THREAD 100

void * thread_inc(void * arg);
void * thread_des(void * arg);
long long num = 0; // global(전역변수) => 정적 데이터 영역에 저장됨 

int main(int argc, char *argv[]) // 실행되면 스택에 main함수 저장
{
	pthread_t thread_id[NUM_THREAD];
	int i; // local(지역변수)

	printf("sizeof long long: %d \n", sizeof(long long)); //printf() 같은 함수도 다 스택에 printf() 이름으로 저장(할당)되는거
	for(i = 0; i<NUM_THREAD; i++)
	{
		if(i%2) pthread_create(&(thread_id[i]), NULL, thread_inc, NULL);
		else pthread_create(&(thread_id[i]), NULL, thread_des, NULL);
	}

	for(i=0; i<NUM_THREAD; i++)
		pthread_join(thread_id[i], NULL);

	printf("result: %lld \n", num);
	return 0;
}

void * thread_inc(void * arg)
{
	int i;
	for(i=0; i<50000000; i++)
		num+=1;
	return NULL;
}

void * thread_des(void * arg)
{
	int i;
	for(i=0; i<50000000; i++)
		num-=1;
	return NULL;
}

