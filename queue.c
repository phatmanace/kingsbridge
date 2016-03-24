#include "queue.h"
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include "zlog.h"
#include <pthread.h>

/*
 * Create new comment node with nothing
 */
QH* newQueue()
{
	QH* head = malloc(sizeof(QH));
	head->size  = 0;
	head->queue = NULL;

	return head;
}

QI* newQueueItem(int i)
{
	QI* item = malloc(sizeof(QI));
	if(item == NULL){
		printf("ALARM ALARM.. MAlloc() failed\n");
		return NULL;
	}
	item->id = i;
	item->next = NULL;
	item->previous = NULL;

	return item;
}

void QueueFree(QI** node)
{
	if (node == NULL){
		return;
	}
	free(*node);
	*node = NULL;
	

}

void QueueEntireClear(QH* head)
{

	QI* tmp = head->queue;
	if(tmp == NULL)
	{
		return;	
	}
	while(tmp->next != NULL){
		tmp = tmp->next;
	}
	
	QI* prev = tmp;
	while(prev != NULL){
		prev = tmp->previous;
		QueueFree(&tmp);
		tmp = prev;
	}
}
QI* SearchQueue(const QI* node, int id)
{
	QI* found = NULL;

	if (node == NULL){
		return (QI*)found;
	}

	if (node->id == id)
        {
		return (QI*)node;
	}
	if ((found = SearchQueue(node->next, id)) != NULL){
		return found;
	}
	return found;
}
void PrintQueue(QH* head){
	
	int i = 0;
	if(head == NULL || head->queue == NULL){
		printf("               ------------EMPTY QUEUE---------------------\n");
		return;		
	}
	QI* tmp = head->queue;
		printf("                    --> ID @ %d is %d\n", i, tmp->id);
	
	while (tmp->next != NULL){
		printf("                    --> ID @ %d is %d\n", i, tmp->id);
		tmp = tmp->next;
		i++;
	}
		printf("               -----------END OF Print Queue-----------------\n");

}
int QpopItem(QH* head, pthread_mutex_t *lock)
{
	pthread_mutex_lock(lock);
	if (head == NULL || head->queue == NULL){
		return -1;
	}
	int id = -1;
	PrintQueue(head);

	if(QSize(head) == 1){
		printf("\nQuick exit for queue size 1\n");
		id = head->queue->id;
		QueueFree(&(head->queue));
		head->queue=NULL;
		pthread_mutex_unlock(lock);
		PrintQueue(head);
		return id;
	}
		
	
	QI* tmp = head->queue;
	QI* prev = head->queue;
	//printf("Setting #1 to %p and #p to %p\n", tmp, prev);
	while (tmp->next != NULL){
		prev = tmp;
		tmp = tmp->next;
		//printf(" --> Setting #1 to %p and #p to %p\n", tmp, prev);
		
	}
	id = tmp->id;
	QueueFree(&tmp);
	prev->next = NULL;
	PrintQueue(head);
	pthread_mutex_unlock(lock);
	return id;
}

int QAppendItem(QH* head, int newId, pthread_mutex_t *lock)
{
	if (head == NULL){
		return -1;
	}
	pthread_mutex_lock(lock);
	if(head->queue == NULL){
		printf("1st time queueing\n");
		QI* _n = newQueueItem(newId);
		head->queue = _n;
		PrintQueue(head);
		pthread_mutex_unlock(lock);
		return 0;
	}
	if(QFindById(head->queue, newId) != NULL){
		pthread_mutex_unlock(lock);
		return -1;
	}
	QI* tmp = head->queue;
	while (tmp->next != NULL){
		tmp = tmp->next;
	}
	QI* _n = newQueueItem(newId);
	if(_n == NULL){
		printf("Something went horribly wrong\n");
		pthread_mutex_unlock(lock);
		return -1;
	}
	_n->previous = tmp;
	tmp->next = _n;
	_n->next = NULL;
	PrintQueue(head);
	pthread_mutex_unlock(lock);
	return 0;
}


int QSize(QH* head)
{
	int i = 0;
	if (head == NULL){
		return -1;
	}
	if(head == NULL || head->queue == NULL){
		return 0;
	}
	QI* tmp = head->queue;

	while (tmp != NULL) {
		i++;
		printf("Size is now %d\n", i);
		tmp = tmp->next;
	}
	return i;
}


QI* QFindById(QI* node, int id)
{
	QI* tmp = node;

	if (tmp == NULL) {
		return 0;
	}
	while (tmp != NULL) {
		if (tmp->id == id) {
			return tmp;
		}
		tmp = tmp->next;
	}
	return NULL;
}

