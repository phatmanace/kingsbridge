
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <pthread.h>
#include <unistd.h>

#include "queue.h"

#define RUNS 10000


pthread_mutex_t	lock;

void *appendLoop(void *arg){
	ND** q = (ND**)arg;
	long ct = 0;
	while(ct < RUNS){
			int _sleep = rand() % 10;
		if(Size(*q) != 0){
			AppendItem(*q, _sleep,  &lock);
			ct++;
			usleep(_sleep * 10);
		}else{
			(*q) = newQueue(5);
		}
	}
	return arg;

}
void *appendLoop2(void *arg){
	ND** q = (ND**)arg;
	long ct = 0;
	while(ct < RUNS){
			int _sleep = rand() % 10;
		if(Size(*q) != 0){
			AppendItem(*q, _sleep,  &lock);
			ct++;
			usleep(_sleep * 10);
		}else{
			(*q) = newQueue(5);
		}
	}
	return arg;

}

int main(void)
{
	
	pthread_t tid1, tid2;
	printf("Starting Queue test\n");
	pthread_mutex_init(&lock, NULL);
	ND* queue = newQueue(5);
	printf("Size is %d\n", Size(queue));
	AppendItem(queue, 4, &lock);
	printf("Size is %d\n", Size(queue));
	int pop = popItem(&queue, &lock);
	printf("Size after popping (%d) is %d => %p \n", pop, Size(queue), queue);
	 pop = popItem(&queue, &lock);
	printf("Size after popping (%d) is %d => %p \n", pop, Size(queue), queue);
	AppendItem(queue, 6, &lock);
	printf("Size (of hopefullly null queue is) %d\n", Size(queue));
	AppendItem(queue, 4, &lock);
	printf("Size after append is %d\n", Size(queue));
	queue = newQueue(5);
	AppendItem(queue, 4, &lock);
	printf("Size after append is %d\n", Size(queue));
	printf("Completed Queue test\n");
	pthread_create(&tid1, NULL, appendLoop, &queue);
	pthread_create(&tid2, NULL, appendLoop2, &queue);
	pthread_join(tid1, NULL);
	pthread_join(tid2, NULL);
	printf("Size after thread finish is %d\n", Size(queue));
	pthread_mutex_destroy(&lock);
	return 0;
}
