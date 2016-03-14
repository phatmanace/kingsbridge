#include "queue.h"
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include "zlog.h"

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



void TreeFree(ND* node)
{
	if (node == NULL)
		return;
	free(node);

}
ND* SearchTree(const ND* node, int id)
{
	ND* found = NULL;

	if (node == NULL){
		return (ND*)found;
	}

	if (node->id == id)
        {
		return (ND*)node;
	}
	if ((found = SearchTree(node->next, id)) != NULL){
		return found;
	}
	return found;
}


int AppendItem(ND* node, ND* newNode)
{
	if (node == NULL)
		return -1;
	ND* tmp = node;
	while (tmp->next != NULL)
		tmp = tmp->next;
	tmp->next = newNode;
	return 0;
}



int SiblingCount(const ND* node)
{
	if (node == NULL)
		return 0;
	return Size(node->next);
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

