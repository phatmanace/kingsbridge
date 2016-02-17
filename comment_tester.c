
#include "comment_tree.h"
#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <string.h>
#include "ncurses.h"
#include <unistd.h>

#define max(a, b) (a > b ? a : b)

void dump(comment_item_tree *tree, const char* title)
{

	printf("===============%s(%s)===============\n", title, tree->text);
	printf("     Initial sibling count is   %d\n",  SiblingCount(tree));
	printf("     Total Node Count           %d\n", TotalNodeCount(tree));
	printf("     Total Size(Resursive)      %d\n", TotalSize(tree));
	printf("     Child Count                %d\n", ChildCount(tree, false));
	printf("     Total Size At this level:  %d\n", Size(tree));
	printf("===============---==================\n");
}

int main(void)
{
	char str[7] = "1.2.3.4";
	char *token;

	token = strtok(str, ".");
	while (token != NULL) {
		printf("Token... %s\n", token);
		token = strtok(NULL, ".");
	}
	printf("Starting comment tree handling test\n");
	int i = 0;
	comment_item_tree* root = newCommentTreeNodeWithText("_ROOT", i++);
	comment_item_tree* tree = newCommentTreeNodeWithText("Lev1-Initial", i++);
	AppendChild(root, tree);
	printf("comments in initial node are %s\n", tree->text);
	dump(tree, "initial state");
	assert(SiblingCount(tree)   == 0);
	assert(TotalNodeCount(tree) == 1);
	printf("Before append, %p, %p\n", tree, tree->next);
	printf("Appending item: %d\n", AppendItem(tree, newCommentTreeNodeWithText("Lev1-first", i++)));
	printf("Appending CHILD item: %d\n", AppendChild(tree, newCommentTreeNodeWithText("Level2-cc-1", i++)));
	printf("Appending CHILD item: %d\n", AppendChild(tree, newCommentTreeNodeWithText("Level2-cc-2", i++)));
	printf("Appending CHILD item: %d\n", AppendChild(tree, newCommentTreeNodeWithText("Level2-cc-3", i++)));
	printf("Appending CHILD item: %d\n", AppendChild(tree, newCommentTreeNodeWithText("Level2-cc-4", i++)));
	printf("Appending CHILD item: %d\n", AppendChild(tree, newCommentTreeNodeWithText("Level2-cc-5", i++)));
	printf("Appending CHILD item: %d\n", AppendChild(tree->children->next, newCommentTreeNodeWithText("Third Level", i++)));
	printf("Appending item: %d\n", AppendItem(tree, newCommentTreeNodeWithText("Lev2-Second", i++)));
	printf("Appending item: %d\n", AppendItem(tree, newCommentTreeNodeWithText("Lev2-Third", i++)));
	printf("After append, %p, %p\n", tree, tree->next);
	dump(tree, "after_append");
	PrintTree(root, PRINT_ALL_TREE);
	printf("Tree pointer is now %p, %p\n", root, root->next);


	if (SearchTree(root, "Third Level")) {
		printf("Search Succeded\n");
	}else{
		printf("Search Failed\n");
	}
	PrintTree(root, PRINT_ALL_TREE);
	printf("----------------printing only the subtre------------------\n");
	PrintTree(root, PRINT_ONLY_EXPANDED_NODES);
	SetExpansionState(root, FALSE);
	if (SearchTree(root, "Lev2-Second")) {
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
	if (FindById(root, 4) != NULL) {
		printf("locate by ID worked...\n");
	}else{
		printf("locate by ID failed...\n");
	}

	if (FindById(root, 40) == NULL) {
		printf("locate by ID worked (not_found)...\n");
	}else{
		printf("locate by ID failed..(found when unexpected).\n");
	}
	char* _foo = "It is unbelievable how invasive Adobe software is. I try very, very hard to avoid installing anything by Adobe, because I know that the software will do whatever it wants on my drive, install itself all over the place, install various (ugly) menubar items which are non-removable, run things at login, make a Creative Cloud Files on my Desktop (!), make me a member of the Community, demand my password to get administrative privileges, without explaining what for, make lots of network connections to various servers, sending unknown information about me, run various processes in the background (AAM Updates Notifier, anyone?) and run various updaters with annoying popups whenever it feels like it. I think CC will also pester you with upselling popups, or new features information. \n Back when Flash was still a thing, they got so low as to try to sneak in more of their products when I just wanted to update Flash.\n  It's crappy behavior and should be called out for being what it is. We are too quiet about it and we got so used to taking crap from software producers that many people don't even complain.";

	int start = 0;
	int stop = 0;
	bool had_newline  = false;
	bool keepscanning = true;
	int span = 80;
	int lastspace = 0;

	printf("                                  __________________________\n");
	while (keepscanning) {

		// compute stop
		while ((stop - start) < span) {
			stop++;
			if (_foo[stop] == ' ') {
				lastspace = stop;
			}
			if (_foo[stop] == '\n') {
				had_newline = true;
				stop--;
				break;
			}
			if (stop == strlen(_foo)) {
				keepscanning = false;
				break;
			}
		}
                /*
		printf("----                        (NL%d)(LS%4d)(ST%4d)(SP%4d)|%.*s\n",
		       had_newline,
		       lastspace,
		       start,
		       stop,
		       (stop - start),
		       _foo + start);
                */
		if (had_newline == false) {
			stop = lastspace + 1;
		}
                if(had_newline == true){
                    while(_foo[start] == ' '){
                        start++;
                    }
                }
		printf("                            (NL%d)(LS%4d)(ST%4d)(SP%4d)|%.*s\n",
		       had_newline,
		       lastspace,
		       start,
		       stop,
		       (stop - start),
		       _foo + start);
		if (had_newline) {
			printf("_NEWLINE_                   (%d)(%4d)(%4d)--%.*s\n",
			       had_newline, lastspace, (stop),  span,  " ");
		}
		start = stop;
		if (had_newline) {
			start += 2;
		}
		had_newline = false;
		stop++;
		usleep(200 * 1000);



	}
	printf("                      LL=>  (%4d) |%.*s\n", lastspace, (int)( strlen(_foo) - start),  _foo + start);
	printf("                                  __________________________\n");
	printf(" %d => %d/%zu\n", start, span, strlen(_foo));


	return 0;
}
