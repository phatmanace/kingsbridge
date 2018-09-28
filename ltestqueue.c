
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <pthread.h>
#include <unistd.h>

#include "queue.h"

#define RUNS 10000


pthread_mutex_t lock;


void *appendLoop(void *arg)
{
	QH* q = (QH*)arg;
	long ct = 0;

	while (ct < RUNS) {
		int _sleep = rand() % 10;
		QAppendItem(q, _sleep,  &lock);
		ct++;
		usleep(_sleep * 10);
	}
	return arg;

}

void *appendLoop2(void *arg)
{
	QH* q = (QH*)arg;
	long ct = 0;

	while (ct < RUNS) {
		int _sleep = rand() % 10;
		QAppendItem(q, _sleep,  &lock);
		ct++;
		usleep(_sleep * 10);
	}
	return arg;

}

int main(void)
{

	int pop = 0;
	pthread_t tid1, tid2;

	printf("Starting Queue test\n");
	pthread_mutex_init(&lock, NULL);
	QH* queue = newQueue();
	printf("QSize is %d\n", QSize(queue));
	QAppendItem(queue, 4, &lock);
	printf("QSize is %d\n", QSize(queue));
	pop = QpopItem(queue, &lock);
	printf("QSize after popping (%d) is %d => %p \n", pop, QSize(queue), queue);
	pop = QpopItem(queue, &lock);
	printf("QSize after popping (%d) is %d => %p \n", pop, QSize(queue), queue);
	QAppendItem(queue, 6, &lock);
	printf("QSize (of hopefullly null queue is) %d\n", QSize(queue));
	QAppendItem(queue, 4, &lock);
	printf("QSize after append is %d\n", QSize(queue));
	queue = newQueue(5);
	QAppendItem(queue, 4, &lock);
	printf("QSize after append is %d\n", QSize(queue));
	printf("Completed Queue test\n");
	pthread_create(&tid1, NULL, appendLoop, queue);
	pthread_create(&tid2, NULL, appendLoop2, queue);
	pthread_join(tid1, NULL);
	pthread_join(tid2, NULL);
	printf("QSize after thread finish is %d\n", QSize(queue));
	pthread_mutex_destroy(&lock);
	QueueEntireClear(queue);
	free(queue);
	return 0;
}
