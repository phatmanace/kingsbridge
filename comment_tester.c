
#include "comment_tree.h"
#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <string.h>
#include "ncurses.h"

void dump(comment_item_tree *tree, const char* title){

    printf("===============%s(%s)===============\n", title, tree->text);
    printf("     Initial sibling count is   %d\n",  SiblingCount(tree));
    printf("     Total Node Count           %d\n", TotalNodeCount(tree));
    printf("     Total Size(Resursive)      %d\n", TotalSize(tree));
    printf("     Child Count                %d\n", ChildCount(tree, false));
    printf("     Total Size At this level:  %d\n", Size(tree));
    printf("===============---==================\n");
}

int main(void){
    char str[7] = "1.2.3.4";
    char *token;
    token = strtok(str, ".");
    while(token != NULL){
        printf("Token... %s\n", token);
        token = strtok(NULL, ".");
    }
    printf("Starting comment tree handling test\n");
    comment_item_tree* root = newCommentTreeNodeWithText("_ROOT");     
    comment_item_tree* tree = newCommentTreeNodeWithText("Lev1-Initial");     
    AppendChild(root, tree);
    printf("comments in initial node are %s\n", tree->text); 
    dump(tree, "initial state");
    assert(SiblingCount(tree)   == 0);
    assert(TotalNodeCount(tree) == 1);
    printf("Before append, %p, %p\n", tree, tree->next);
    printf("Appending item: %d\n", AppendItem(tree, newCommentTreeNodeWithText("Lev1-first"))); 
    printf("Appending CHILD item: %d\n", AppendChild(tree, newCommentTreeNodeWithText("Level2-cc-1"))); 
    printf("Appending CHILD item: %d\n", AppendChild(tree, newCommentTreeNodeWithText("Level2-cc-2"))); 
    printf("Appending CHILD item: %d\n", AppendChild(tree, newCommentTreeNodeWithText("Level2-cc-3"))); 
    printf("Appending CHILD item: %d\n", AppendChild(tree, newCommentTreeNodeWithText("Level2-cc-4"))); 
    printf("Appending CHILD item: %d\n", AppendChild(tree, newCommentTreeNodeWithText("Level2-cc-5"))); 
    printf("Appending CHILD item: %d\n", AppendChild(tree->children->next, newCommentTreeNodeWithText("Third Level"))); 
    printf("Appending item: %d\n", AppendItem(tree, newCommentTreeNodeWithText("Lev2-Second"))); 
    printf("Appending item: %d\n", AppendItem(tree, newCommentTreeNodeWithText("Lev2-Third"))); 
    printf("After append, %p, %p\n", tree, tree->next);
    dump(tree, "after_append");
    PrintTree(root, PRINT_ALL_TREE);
    printf("Tree pointer is now %p, %p\n", root, root->next);

    if(SearchTree(root, "Third Level"))
    {
        printf("Search Succeded\n");
    }else{
        printf("Search Failed\n");
    }
    PrintTree(root, PRINT_ALL_TREE);
    printf("----------------printing only the subtre------------------\n");
    PrintTree(root, PRINT_ONLY_EXPANDED_NODES);
    SetExpansionState(root, FALSE);
    if(SearchTree(root, "Lev2-Second"))
    {
        printf("Search Succeded\n");
    }else{
        printf("Search Failed\n");
    }
    printf("\xc2\xAE\n");
    PrintTree(root, PRINT_ALL_TREE);
    printf("----------------printing only the subtre------------------\n");
    PrintTree(root, PRINT_ONLY_EXPANDED_NODES);
    printf("----------------set expansion state to true---------------\n");
    SetExpansionState(root, TRUE);
    PrintTree(root, PRINT_ALL_TREE);
    printf("----------------set expansion state to false--------------\n");
    SetExpansionState(root, FALSE);
    dump(root, "After Everything");
    int sizeTotal = TotalSize(root);
    printf("Total size is %d\n", sizeTotal);
    PrintTree(root, PRINT_ALL_TREE);
    assert(SiblingCount(root)   == 0);
    assert(TotalSize(root) == 11);
    printf("all done... Tests all pass!\n");

    return 0; 
}
