#ifndef QUEUE_H
#define QUEUE_H
#include <stdbool.h>
#include "zlog.h"
#include <pthread.h>
#define QI queueitem
#define QH queuehead


typedef struct QI {
	int id;
	struct QI* next;
	struct QI* previous;
} QI;

typedef struct QH {
	int size;
	QI* queue;
} QH;

QH* newQueue();

void QueueFree(QI** node);

QI* SearchQueue(const QI* node, int id);

int QpopItem(QH* head, pthread_mutex_t *lock);

int QAppendItem(QH* head, int newId, pthread_mutex_t *lock);

int QSiblingCount(const QI* node);

int QSize(QH* node);

void PrintQueue(QH* head);

QI* QFindById(QI* node, int id);


void QueueEntireClear(QH* head);

#endif
