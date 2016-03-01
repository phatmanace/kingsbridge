#include <stdbool.h>
#include "zlog.h"
#define ND comment_item_tree
typedef enum {
	PRINT_ALL_TREE = 0,
	PRINT_ONLY_EXPANDED_NODES
}node_method;
typedef struct ND {
	char *text;
	struct ND* next;
	struct ND* previous;
	struct ND* children;
	struct ND* parent;
	int flags;
	int id;
	int _ft_depth;
} ND;

ND* newCommentTreeNode(int id);
ND* newCommentTreeNodeWithText(char* text, int id);

int  SiblingCount(const ND* node);
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
bool SearchTree(ND* node, char* searchString);
void SetExpansionState(ND* node, bool expanded);
int  TreeMaxDepth(const ND* node);
ND* GetNextTreeElement(ND* node);
void SetSingleExpansionState(ND* node, bool expanded);
ND **ToFlatTree(ND *node, int* n);
int ComputedDepth(ND* node);

#define NODE_EXPANDED 1
#define NODE_LOCATED  2
