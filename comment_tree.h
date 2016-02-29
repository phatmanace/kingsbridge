#include <stdbool.h>
#include "zlog.h"
typedef enum {
	PRINT_ALL_TREE = 0,
	PRINT_ONLY_EXPANDED_NODES
}node_method;
typedef struct comment_item_tree {
	char *text;
	struct comment_item_tree* next;
	struct comment_item_tree* previous;
	struct comment_item_tree* children;
	struct comment_item_tree* parent;
	int flags;
	int id;
	int _ft_depth;
} comment_item_tree;

comment_item_tree* newCommentTreeNode(int id);
comment_item_tree* newCommentTreeNodeWithText(char* text, int id);

int  SiblingCount(const comment_item_tree* node);
comment_item_tree* FindById(comment_item_tree* node, int id);
int  Size(comment_item_tree* node);
int  TotalSize(comment_item_tree* node);
int  VisibleSize(comment_item_tree* node);
int  TotalNodeCount(const comment_item_tree* node);
int  ChildCount(const comment_item_tree* node, bool recurse);
int  AppendItem(comment_item_tree* node, comment_item_tree* newNode);
int  AppendChild(comment_item_tree* node, comment_item_tree* newNode);
int  PrintTree(comment_item_tree* node, node_method print_method);
int  LogPrintTree(zlog_category_t* c, comment_item_tree* node, node_method print_method);
bool isExpanded(comment_item_tree* node);
bool SearchTree(comment_item_tree* node, char* searchString);
void SetExpansionState(comment_item_tree* node, bool expanded);
int  TreeMaxDepth(const comment_item_tree* node);
comment_item_tree* GetNextTreeElement(comment_item_tree* node);
void SetSingleExpansionState(comment_item_tree* node, bool expanded);
comment_item_tree **ToFlatTree(comment_item_tree *node, int* n);

#define NODE_EXPANDED 1
#define NODE_LOCATED  2
