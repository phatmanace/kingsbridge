#include "comment_tree.h"
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include "zlog.h"

void LogPrintTreeItem(zlog_category_t* c, const ND* node, int offset, int *counter,  node_method method);
/*
 * Create new comment node with nothing
 */
ND* newCommentTreeNode(int id)
{
	ND* item = malloc(sizeof(ND));

	item->children = NULL;
	item->next     = NULL;
	item->previous = NULL;
	item->parent   = NULL;
	item->flags    = 0;
	item->id       = id;
	item->_ft_depth = 0;
	SetSingleExpansionState(item, false);
	return item;
}

ND* newCommentTreeNodeWithText(char* text, int id)
{
	ND* item = newCommentTreeNode(id);
	item->text       = malloc(strlen(text));
	strcpy(item->text, text);
	return item;
}


bool isExpanded(ND* node)
{
	return (node->flags >> NODE_EXPANDED) & 1;
}

void MarkParentsExpanded(ND* node)
{
	if (node == NULL)
		return;
	node->flags |= 1 << NODE_EXPANDED;
	if (node->parent != NULL)
		MarkParentsExpanded(node->parent);
}

void SetSingleExpansionState(ND* node, bool expanded)
{
	if (expanded) {
		node->flags |= 1 << NODE_EXPANDED;
	}else{
		node->flags &= ~(1 << NODE_EXPANDED);
	}
}

void SetExpansionState(ND* node, bool expanded)
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

void TreeFree(ND* node)
{
	if (node == NULL)
		return;
	free(node->text);
	free(node);

}
bool SearchTree(ND* node, char* searchString)
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

int ComputedDepth(ND* node){
	
	int d = 0;
	ND* tmp = node;
	while(tmp->parent != NULL){
		d++;
		tmp = tmp->parent;
	}
	return d;
}

int AppendItem(ND* node, ND* newNode)
{
	if (node == NULL)
		return -1;
	ND* tmp = node;
	while (tmp->next != NULL)
		tmp = tmp->next;
	tmp->next = newNode;
	newNode->previous = tmp;
	newNode->parent = tmp->parent;
	return 0;
}

ND* GetNextTreeElement(ND* node)
{
	if (node == NULL) {
		return NULL;
	}
	if (node->children != NULL && isExpanded(node)) {
		return node->children;
	}
	if (node->next == NULL) {
		return node->parent;
	}
	return NULL;

}
int AppendChild(ND* node, ND* newNode)
{
	if (node == NULL)
		return -1;
	if (node->children != NULL) {
		ND* tmp = node->children;
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

void LogPrintTreeItem(zlog_category_t* c, const ND* node, int offset, int *counter,  node_method method)
{
	int newoffset = offset + 1;
	int i = 0;
	ND* tmp = (ND*)node;

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
			if (firstChild) {
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
		if (method == PRINT_ALL_TREE || isExpanded(tmp)) {
			zlog_info(c, "%d[dp/%d - %5d] %s %d (i=%d) F/%d) -> %25s %p, %p, %p"
				  , isExpanded(tmp)
				  , offset
				  , *counter
				  , prefix
				  , TotalSize(tmp)
				  , tmp->id
				  , tmp->flags
				  , tmp->text
				  , tmp
				  , tmp->next
				  , tmp->previous
				  );
		}
		if (ChildCount(tmp, false) > 0) {
			LogPrintTreeItem(c, tmp->children, newoffset, counter, method);
		}
		tmp = tmp->next;
		firstChild = false;
		free(prefix);
	}


}

int LogPrintTree(zlog_category_t* c, ND* node, node_method method)
{
	if (node == NULL) {
		printf("Whole tree was null");
		return -1;
	}
	int y = 0;
	zlog_info(c, "________________________________________________");
	LogPrintTreeItem(c, node, 0, &y,  method);
	zlog_info(c, "________________________________________________");
	return 0;
}
void PrintTreeItem(const ND* node, int offset, int *counter,  node_method method)
{
	int newoffset = offset + 1;
	int i = 0;
	ND* tmp = (ND*)node;

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
			if (firstChild) {
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
		printf("[%d - %d -  %2d] %s %-25s TS=%4d (id=%d) F=%d) (CC_nr=%d, CC_R=%d, T=%d)\n"
		       , offset
		       , isExpanded(tmp)
		       , *counter
		       , prefix
		       , tmp->text
		       , TotalSize(tmp)
		       , tmp->id
		       , tmp->flags
		       , ChildCount(tmp, false)
		       , ChildCount(tmp, true)
		       , TotalNodeCount(tmp)
		       );
		if (ChildCount(tmp, false) > 0 && (method == PRINT_ALL_TREE || isExpanded(tmp))) {
			PrintTreeItem(tmp->children, newoffset, counter, method);
		}
		tmp = tmp->next;
		firstChild = false;
		free(prefix);
	}


}

int PrintTree(ND* node, node_method method)
{
	if (node == NULL) {
		printf("Whole tree was null");
		return -1;
	}
	int y = 0;
	PrintTreeItem(node, 0, &y,  method);
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

int VisibleSize(ND* node)
{
	int i = 0;
	ND* tmp = node;

	if (tmp == NULL) {
		return 0;
	}
	while (tmp != NULL) {
		i++;
		if (isExpanded(tmp) && tmp->children != NULL ) {
			i += VisibleSize(tmp->children);
		}
		tmp = tmp->next;
	}
	return i;
}

void _flatTreeInternal(ND **ray, ND *node, int* rayindex, int depth){
       // ray[0] = 
        if(node == NULL){
            return;
        }
        //printf("           Depth=%d, Adding  %s at index %d\n", depth,  node->text, *rayindex);
        node->_ft_depth = depth;
        ray[*rayindex] = (node);
                (*rayindex)++;

        if(*rayindex > 1000){
            //printf("Error... exiting..\n");
            return;
        }

        ND* tmp = node;
                if(tmp->children != NULL && isExpanded(tmp)){
                    //printf("going into children at %s\n", tmp->text);
                    _flatTreeInternal(ray, tmp->children, rayindex, depth + 1);
                }
                //printf("Allocating at depth=%d counter=%d %s\n", depth, *rayindex, tmp->text);
                tmp = tmp->next;
                _flatTreeInternal(ray, tmp, rayindex , depth);

}
ND **ToFlatTree(ND *node, int* n)
{

	int _sz = VisibleSize(node);
        *n = _sz ;

	if (_sz == 0) {
		return NULL;
	}
        int _counter = 0;
        
	ND** ray = malloc(sizeof(ND) *  _sz );
        _flatTreeInternal(ray, node, &_counter, 0);
	return ray;
}

int TotalSize(ND* node)
{
	int i = 0;
	ND* tmp = node;

	if (tmp == NULL) {
		return 0;
	}
	while (tmp != NULL) {
		i++;
		if (tmp->children != NULL ) {
			i += TotalSize(tmp->children);
		}
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
		if (tmp->children != NULL ) {
			ND* child = NULL;
			child = FindById(tmp->children, id);
			if (child != NULL) {
				return child;
			}
		}
		tmp = tmp->next;
	}
	return NULL;
}

int _TreeMaxDepth(const ND* node, int depth)
{
	if (node == NULL) {
		return 0;
	}
	if (node->children == NULL) {
		return depth;
	}
	int soFar = 0;
	ND* tmp = (ND*)node->children;
	while (tmp != NULL) {

		int this_run = _TreeMaxDepth(tmp, depth + 1);
		if (this_run > soFar) {
			soFar = this_run;
		}

		tmp = tmp->next;
	}
	return soFar;
}

int TreeMaxDepth(const ND* node)
{
	if (node == NULL) {
		return 0;
	}

	int soFar = 1;
	ND* tmp = (ND*)node;
	while (tmp != NULL) {

		int this_run = _TreeMaxDepth(tmp, 1);
		if (this_run > soFar) {
			soFar = this_run;
		}

		tmp = tmp->next;
	}
	return soFar;
}

int TotalNodeCount(const ND* node)
{
	if (node == NULL)
		return 0;
	return 1 +  SiblingCount(node) + ChildCount(node, true);
}

int ChildCount(const ND* node, bool recurse)
{
	if (node == NULL || node->children == NULL)
		return 0;
	int i = 0;
	const ND* tmp = node->children;
	while (tmp != NULL) {
		i++;
		if (tmp->children != NULL && recurse == true) {
			i += TotalSize(tmp->children);
		}
		tmp = tmp->next;
	}
	return i;
}
