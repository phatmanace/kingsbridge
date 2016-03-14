#include <stdbool.h>
#include "zlog.h"
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
void TreeFree(ND* node);
ND* SearchTree(const ND* node, int id);
int AppendItem(ND* node, ND* newNode);
int SiblingCount(const ND* node);

int Size(ND* node);

ND* FindById(ND* node, int id);


#define NODE_EXPANDED 1
#define NODE_LOCATED  2
