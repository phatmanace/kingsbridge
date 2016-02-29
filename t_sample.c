
#include "comment_tree.h"
#include "t_sample.h"
#include <stdio.h>
#include <stdlib.h>

#define max(a, b) (a > b ? a : b)


comment_item_tree* make_sample_tree()
{
	int i = 0;
	comment_item_tree* root = newCommentTreeNodeWithText("ROOT_OF_TREE", i++);
	comment_item_tree* lev1 = newCommentTreeNodeWithText("Lev1-ROOT", i++);

	SetSingleExpansionState(root, true);
	SetSingleExpansionState(lev1, true);
	AppendChild(root, lev1);
	AppendItem(lev1, newCommentTreeNodeWithText("1st - one", i++));
	AppendChild(lev1, newCommentTreeNodeWithText("1st-1", i++));
	AppendChild(lev1, newCommentTreeNodeWithText("1st-2", i++));
	AppendChild(lev1, newCommentTreeNodeWithText("1st-3", i++));
	AppendChild(lev1, newCommentTreeNodeWithText("1st-4", i++));
	AppendChild(lev1, newCommentTreeNodeWithText("1st-5", i++));
	AppendChild(lev1->children->next, newCommentTreeNodeWithText("2nd Level-1", i++));
	AppendChild(lev1->children->next, newCommentTreeNodeWithText("2nd Level-2", i++));
	AppendChild(lev1->children->next, newCommentTreeNodeWithText("2nd Level-3", i++));
	AppendItem(lev1, newCommentTreeNodeWithText("1st-6", i++));
	AppendItem(lev1, newCommentTreeNodeWithText("1st-7", i++));

	return root;
}
