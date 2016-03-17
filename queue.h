#include <stdbool.h>
#include "zlog.h"
#include <pthread.h>
#define ND queueitem
typedef enum {
	PRINT_ALL_TREE = 0,
	PRINT_ONLY_EXPANDED_NODES
}node_method;
typedef struct ND {
	int id;
	struct ND* next;
} ND;


ND* newQueue(int id);
void QueueFree(ND** node);
ND* SearchQueue(const ND* node, int id);
int popItem(ND** queue, pthread_mutex_t *lock);
int AppendItem(ND* node, int newId, pthread_mutex_t *lock);
int SiblingCount(const ND* node);

int Size(ND* node);

ND* FindById(ND* node, int id);


#define NODE_EXPANDED 1
#define NODE_LOCATED  2
