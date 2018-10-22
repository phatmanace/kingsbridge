
#include "comment_tree.h"
#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <string.h>
#include "ncurses.h"
#include <unistd.h>
#include "t_sample.h"
#include "time.h"

#define max(a, b) (a > b ? a : b)

void dump(comment_item_tree *tree, const char* title)
{

	printf("===============%s(%s)===============\n", title, tree->text);
	printf("     Initial sibling count is     %d\n",  SiblingCount(tree));
	printf("     Total Node Count             %d\n", TotalNodeCount(tree));
	printf("     Total Size(Resursive)        %d\n", TotalSize(tree));
	printf("     Child Count                  %d\n", ChildCount(tree, false));
	printf("     Total Size At this level:    %d\n", Size(tree));
	printf("     Visible Size At this level:  %d\n", VisibleSize(tree));
	printf("===============---==================\n");
	PrintTree(tree, PRINT_ALL_TREE);
}

int main(void)
{
	printf("Simple Comment tester\n");
	ND* root = nCmmtText("Test Head", 10);
	dump(root, "Initial Dump...");




    AppendChild(root, nCmmtText("Test Child 1", 10));
    AppendChild(root, nCmmtTextTime("Test Child 2", 10, (long)time(NULL)));
    dump(root, "2nd Dump...");
 	return 1;
}
