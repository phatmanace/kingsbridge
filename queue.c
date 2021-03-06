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

void debug(char *msg){
	#ifdef _QDEBUG
	printf("Queue %s\n", msg);
#endif
}
	
QH* newQueue()
{
	QH* head = malloc(sizeof(QH));

	head->size  = 0;
	head->queue = NULL;
	head->dead = false;

	return head;
}
void QMarkDead(QH* node, bool dead, pthread_mutex_t *lock)
{
	if(node == NULL || lock == NULL){
		return;
	}
	pthread_mutex_lock(lock);
	node->dead = dead;
	pthread_mutex_unlock(lock);
}

QI* newQueueItem(int i)
{
	QI* item = malloc(sizeof(QI));

	if (item == NULL) {
		debug("ALARM ALARM.. MAlloc() failed\n");
		return NULL;
	}
	item->id = i;
	item->next = NULL;
	item->previous = NULL;

	return item;
}

void QueueFree(QI** node)
{
	if (node == NULL) {
		return;
	}
	free(*node);
	*node = NULL;


}

void QueueEntireClear(QH* head)
{

	QI* tmp = head->queue;

	if (tmp == NULL) {
		return;
	}
	while (tmp->next != NULL) {
		tmp = tmp->next;
	}

	QI* prev = tmp;
	while (prev != NULL) {
		prev = tmp->previous;
		QueueFree(&tmp);
		tmp = prev;
	}
}
QI* SearchQueue(const QI* node, int id)
{
	QI* found = NULL;

	if (node == NULL) {
		return (QI*)found;
	}

	if (node->id == id) {
		return (QI*)node;
	}
	if ((found = SearchQueue(node->next, id)) != NULL) {
		return found;
	}
	return found;
}
void PrintQueue(QH* head)
{

	#ifdef DEBUG

	int i = 0;
	if (head == NULL || head->queue == NULL) {
		printf("               ------------EMPTY QUEUE---------------------\n");
		return;
	}
	QI* tmp = head->queue;
	printf("                    --> ID @ %d is %d\n", i, tmp->id);

	while (tmp->next != NULL) {
		printf("                    --> ID @ %d is %d\n", i, tmp->id);
		tmp = tmp->next;
		i++;
	}
	printf("               -----------END OF Print Queue-----------------\n");
	#endif

}
int QpopItem(QH* head, pthread_mutex_t *lock)
{
	debug("          MutexPopLock(): Locking\n");
	pthread_mutex_lock(lock);
	if (head == NULL || head->queue == NULL) {
		debug("          MutexPoPLock(Quick): UnLocking\n");
		pthread_mutex_unlock(lock);
		return -1;
	}
	int id = -1;
	PrintQueue(head);

	if (QSize(head) == 1) {
		debug("Quick exit for queue size 1\n");
		id = head->queue->id;
		//QueueFree(&(head->queue));
		head->queue = NULL;
		debug("          MutexPoPLock(Quick): UnLocking\n");
		pthread_mutex_unlock(lock);
		PrintQueue(head);
		return id;
	}


	QI* tmp = head->queue;
	QI* prev = head->queue;
	//printf("Setting #1 to %p and #p to %p\n", tmp, prev);
	while (tmp->next != NULL) {
		prev = tmp;
		tmp = tmp->next;
		//printf(" --> Setting #1 to %p and #p to %p\n", tmp, prev);

	}
	id = tmp->id;
	//QueueFree(&tmp);
	prev->next = NULL;
	PrintQueue(head);
	debug("         MutexPopLock(End): UnLocking\n");
	pthread_mutex_unlock(lock);
	return id;
}

int QAppendItem(QH* head, int newId, pthread_mutex_t *lock)
{
	if (head == NULL) {
		return -1;
	}
	debug("         MutexAppendLock(): Locking\n");
	pthread_mutex_lock(lock);
	debug("         MutexAppendLock(): done Locking\n");
	if (head->queue == NULL) {
		debug("1st time queueing\n");
		QI* _n = newQueueItem(newId);
		head->queue = _n;
		PrintQueue(head);
		pthread_mutex_unlock(lock);
		debug("         MutexAppendLock(): Unlocking\n");
		return 0;
	}
	if (QFindById(head->queue, newId) != NULL) {
		debug("         MutexAppendLock(): Unlocking\n");
		pthread_mutex_unlock(lock);
		return -1;
	}
	QI* tmp = head->queue;
	while (tmp->next != NULL) {
		tmp = tmp->next;
	}
	QI* _n = newQueueItem(newId);
	if (_n == NULL) {
		debug("Something went horribly wrong\n");
		debug("MutexLock(): Unlocking\n");
		pthread_mutex_unlock(lock);
		return -1;
	}
	_n->previous = tmp;
	tmp->next = _n;
	_n->next = NULL;
	PrintQueue(head);
	debug("MutexLock(): Unlocking\n");
	pthread_mutex_unlock(lock);
	return 0;
}


int QSize(QH* head)
{
	int i = 0;

	if (head == NULL) {
		return -1;
	}
	if (head == NULL || head->queue == NULL) {
		return 0;
	}
	QI* tmp = head->queue;

	while (tmp != NULL) {
		i++;
		#ifdef DEBUG
		printf("Size is now %d\n", i);
		#endif
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

