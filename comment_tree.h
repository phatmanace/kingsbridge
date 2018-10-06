#ifndef COMMENT_TREE_H
#define COMMENT_TREE_H
#define SHALLOW_DEPTH false
#define DEEP_DEPTH true

#include <stdbool.h>
#include "zlog.h"
#define ND comment_item_tree
typedef enum {
	PRINT_ALL_TREE = 0,
	PRINT_ONLY_EXPANDED_NODES
}node_method;
typedef struct ND {
	char *text;
	char **links;
	int linkcount;
	struct ND* next;
	struct ND* previous;
	struct ND* children;
	struct ND* parent;
	int flags;
	int id;
	int parentid;
	int _ft_depth;
} ND;

ND* newCommentTreeNode(int id);
ND* newCommentTreeNodeWithText(char* text, int id);

int  SiblingCount(const ND* node);
void ResetFlatTree(ND* node);
ND* FindById(ND* node, int id);
int  Size(ND* node);
int  TotalSize(ND* node);
int  VisibleSize(ND* node);
int  TotalNodeCount(const ND* node);
int  ChildCount(const ND* node, bool recurse);
int  AppendItem(ND* node, ND* newNode);
int  AppendChild(ND* node, ND* newNode);
int  PrintTree(ND* node, node_method print_method);
int  LogPrintTree(zlog_category_t* c, ND* node, node_method print_method);
bool isExpanded(ND* node);
bool TreeExists(ND* node, char* searchString);
ND*  SearchTree(const ND* node, char* searchString);
ND* SearchById(const ND* node, int id);
bool exists(const ND* node, int id);
ND** allocNodeArray(int sz);
void SetExpansionState(ND* node, bool expanded);
int  TreeMaxDepth(const ND* node);
ND* GetNextTreeElement(ND* node);
void SetSingleExpansionState(ND* node, bool expanded);
ND **ToFlatTree(ND *node, int* n);
int ComputedDepth(ND* node);
void buildCommentTree(ND* root, ND** noderay, int szRaySz, int depth);

#define NODE_EXPANDED 1
#define NODE_LOCATED  2
#endif
