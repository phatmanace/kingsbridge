
#include "comment_tree.h"
#include "t_sample.h"
#include <stdio.h>
#include <stdlib.h>

#define max(a, b) (a > b ? a : b)


comment_item_tree* make_sample_tree()
{
	int i = 0;
	comment_item_tree* root = newCommentTreeNodeWithText("_ROOT", i++);
	comment_item_tree* tree = newCommentTreeNodeWithText("Lev1-Initial", i++);
	AppendChild(root, tree);
	AppendItem(tree, newCommentTreeNodeWithText("Lev1-first", i++));
	AppendChild(tree, newCommentTreeNodeWithText("Level2-cc-1", i++));
	AppendChild(tree, newCommentTreeNodeWithText("Level2-cc-2", i++));
	AppendChild(tree, newCommentTreeNodeWithText("Level2-cc-3", i++));
	AppendChild(tree, newCommentTreeNodeWithText("Level2-cc-4", i++));
	AppendChild(tree, newCommentTreeNodeWithText("Level2-cc-5", i++));
	AppendChild(tree->children->next, newCommentTreeNodeWithText("Third Level", i++));
	AppendItem(tree, newCommentTreeNodeWithText("Lev2-Second", i++));
	AppendItem(tree, newCommentTreeNodeWithText("Lev2-Third", i++));

	return root;
}
