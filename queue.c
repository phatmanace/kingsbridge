#include "queue.h"
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include "zlog.h"
#include <pthread.h>

void LogPrintTreeItem(zlog_category_t* c, const ND* node, int offset, int *counter,  node_method method);
/*
 * Create new comment node with nothing
 */
ND* newQueue(int id)
{
	ND* item = malloc(sizeof(ND));

	item->next     = NULL;
	item->id       = id;
	return item;
}


void QueueFree(ND** node)
{
	if (node == NULL){
		return;
	}
	printf("Freeing up node\n");
	free(*node);
	*node = NULL;

}
ND* SearchQueue(const ND* node, int id)
{
	ND* found = NULL;

	if (node == NULL){
		return (ND*)found;
	}

	if (node->id == id)
        {
		return (ND*)node;
	}
	if ((found = SearchQueue(node->next, id)) != NULL){
		return found;
	}
	return found;
}
int popItem(ND** queue, pthread_mutex_t *lock)
{
	if (queue == NULL){
		return -1;
	}
	int id = -1;

	pthread_mutex_lock(lock);
	if(Size(*queue) == 1){
		printf("Quick exit for queue size 1\n");
		id = (*queue)->id;
		QueueFree(queue);
		pthread_mutex_unlock(lock);
		return id;
	}
		

	
	ND* tmp = *queue;
	ND* prev = *queue;
	printf("Setting #1 to %p and #p to %p\n", tmp, prev);
	while (tmp->next != NULL){
		prev = tmp;
		tmp = tmp->next;
		printf(" --> Setting #1 to %p and #p to %p\n", tmp, prev);
		
	}
	id = tmp->id;
	QueueFree(&tmp);
	prev->next = NULL;
	pthread_mutex_unlock(lock);
	return id;
}

int AppendItem(ND* node, int newId, pthread_mutex_t *lock)
{
	if (node == NULL)
		return -1;
	pthread_mutex_lock(lock);
	ND* tmp = node;
	while (tmp->next != NULL)
		tmp = tmp->next;
	tmp->next = newQueue(newId);
	pthread_mutex_unlock(lock);
	return 0;
}


int Size(ND* node)
{
	int i = 0;
	ND* tmp = node;

	if (tmp == NULL)
		return 0;
	while (tmp != NULL) {
		i++;
		tmp = tmp->next;
	}
	return i;
}


ND* FindById(ND* node, int id)
{
	ND* tmp = node;

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

