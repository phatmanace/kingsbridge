#include "comment_tree.h"
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>

/*
 * Create new comment node with nothing
 */
comment_item_tree* newCommentTreeNode(int id)
{
	comment_item_tree* item = malloc(sizeof(comment_item_tree));

	item->children = NULL;
	item->next     = NULL;
	item->previous = NULL;
	item->parent   = NULL;
	item->flags    = 0;
	item->id       = id;
	return item;
}

comment_item_tree* newCommentTreeNodeWithText(char* text, int id)
{
	printf("copying %s\n", text);
	comment_item_tree* item = malloc(sizeof(comment_item_tree));
	item->children   = NULL;
	item->next       = NULL;
	item->previous   = NULL;
	item->parent     = NULL;
	item->text       = malloc(strlen(text));
	item->flags      = 0;
	item->id         = id;
	strcpy(item->text, text);
	return item;
}

bool isExpanded(comment_item_tree* node)
{
	return (node->flags >> NODE_EXPANDED) & 1;
}

void MarkParentsExpanded(comment_item_tree* node)
{
	if (node == NULL)
		return;
	node->flags |= 1 << NODE_EXPANDED;
	if (node->parent != NULL)
		MarkParentsExpanded(node->parent);
}

void SetExpansionState(comment_item_tree* node, bool expanded)
{
	if (expanded)
		node->flags |= 1 << NODE_EXPANDED;
	else
		node->flags &= ~(1 << NODE_EXPANDED);
	if (node->next != NULL)
		SetExpansionState(node->next, expanded);
	if (node->children != NULL)
		SetExpansionState(node->children, expanded);
}

void TreeFree(comment_item_tree* node)
{
	if (node == NULL)
		return;
	free(node->text);
	free(node);

}
bool SearchTree(comment_item_tree* node, char* searchString)
{
	int this_result = 0;

	if (node == NULL)
		return 1 == 0;
	node->flags &= ~(1 << NODE_LOCATED);

	if (strcmp(node->text, searchString) == 0) {
		this_result++;
		node->flags |= 1 << NODE_LOCATED;
		MarkParentsExpanded(node);
	}
	if (SearchTree(node->next, searchString))
		this_result++;
	if (SearchTree(node->children, searchString))
		this_result++;
	return this_result > 0;
}

int AppendItem(comment_item_tree* node, comment_item_tree* newNode)
{
	if (node == NULL)
		return -1;
	comment_item_tree* tmp = node;
	while (tmp->next != NULL)
		tmp = tmp->next;
	tmp->next = newNode;
	newNode->previous = tmp;
	return 0;
}
int AppendChild(comment_item_tree* node, comment_item_tree* newNode)
{
	if (node == NULL)
		return -1;
	if (node->children != NULL) {
		comment_item_tree* tmp = node->children;
		while (tmp->next != NULL)
			tmp = tmp->next;
		tmp->next = newNode;
		newNode->previous = tmp;
		newNode->parent = node;
	}else{
		node->children = newNode;
		newNode->parent = node;
	}
	return 0;
}

void PrintTreeItem(const comment_item_tree* node, int offset, int *counter,  node_method method)
{
	int newoffset = offset + 1;
	int i = 0;
	comment_item_tree* tmp = (comment_item_tree*)node;

	if (tmp == NULL)
		return;
        bool firstChild = true;
	while (tmp != NULL) {
		(*counter)++;
		char* prefix = calloc(20, sizeof(char));
		i++;
		if (offset == 0) {
			strcpy(prefix, "|");
			int y = 0;
			while (y < 18) {
				prefix = strcat(prefix, " ");
				y++;
			}
		}else{
			int y = 0;
			while (y < offset) {
				prefix = strcat(prefix, "  ");
				y++;
			}
                        if(firstChild){
                            prefix = strcat(prefix, "\\");
                        }else{
                            prefix = strcat(prefix, "|_");
                        }
                        y = strlen(prefix);
                            
			while (y < 18) {
				prefix = strcat(prefix, " ");
				y++;
			}
		}
		/*
		   printf("[%d] %s %d(%10p,nx=%10p, prev=%10p,Parent=%10p,F/%d) -> %30s (CC=%d)\n"
		                                              , offset
		                                              , prefix
		                                              , isExpanded(tmp)
		                                              , tmp
		                                              , tmp->next
		                                              , tmp->previous
		                                              , tmp->parent
		                                              , tmp->flags
		                                              , tmp->text
		                                              ,ChildCount(tmp, false)
		                                                );
		 */
		if (method == PRINT_ALL_TREE || isExpanded(tmp)) {
			printf("[%d - %5d] %s %d (i=%d) F/%d) -> %15s (CC_nr=%d, CC_R=%d, T=%d)\n"
			       , offset
			       , *counter
			       , prefix
			       , TotalSize(tmp)
			       , tmp->id
			       , tmp->flags
			       , tmp->text
			       , ChildCount(tmp, false)
			       , ChildCount(tmp, true)
			       , TotalNodeCount(tmp)
			       );
		}
		if (ChildCount(tmp, false) > 0){
			PrintTreeItem(tmp->children, newoffset, counter , method);
	         }		
		tmp = tmp->next;
                firstChild=false;
		free(prefix);
	}


}

int PrintTree(comment_item_tree* node, node_method method)
{
	if (node == NULL) {
		printf("Whole tree was null");
		return -1;
	}
	int y = 0;
	PrintTreeItem(node, 0, &y,  method);
	return 0;
}



int SiblingCount(const comment_item_tree* node)
{
	if (node == NULL)
		return 0;
	return Size(node->next);
}

int Size(comment_item_tree* node)
{
	int i = 0;
	comment_item_tree* tmp = node;

	if (tmp == NULL)
		return 0;
	while (tmp != NULL) {
		i++;
		tmp = tmp->next;
	}
	return i;
}

int TotalSize(comment_item_tree* node)
{
	int i = 0;
	comment_item_tree* tmp = node;

	if (tmp == NULL){
		return 0;
        }		
	while (tmp != NULL) {
		i++;
		if(tmp->children != NULL ){
			i += TotalSize(tmp->children);
		}
		tmp = tmp->next;
	}
	return i;
}

comment_item_tree* FindById(comment_item_tree* node, int id)
{
	comment_item_tree* tmp = node;

	if (tmp == NULL){
		return 0;
        }		
	while (tmp != NULL) {
		if(tmp->id == id){
			return tmp;
		}
		if(tmp->children != NULL ){
			comment_item_tree* child = NULL;
			child = FindById(tmp->children, id);
			if(child != NULL){
				return child;
			}
		}
		tmp = tmp->next;
	}
	return NULL;
}

int TotalNodeCount(const comment_item_tree* node)
{
	if (node == NULL)
		return 0;
	return 1 +  SiblingCount(node) + ChildCount(node, true);
}

int ChildCount(const comment_item_tree* node, bool recurse)
{
	if (node == NULL || node->children == NULL)
		return 0;
	int i = 0;
	const comment_item_tree* tmp = node->children;
	while (tmp != NULL) {
		i++;
		if(tmp->children != NULL && recurse == true){
			i += TotalSize(tmp->children);
		}
		tmp = tmp->next;
	}
	return i;
}
