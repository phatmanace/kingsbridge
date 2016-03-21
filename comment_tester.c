
#include "comment_tree.h"
#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <string.h>
#include "ncurses.h"
#include <unistd.h>
#include "t_sample.h"

#define max(a, b) (a > b ? a : b)

void dump(comment_item_tree *tree, const char* title)
{

	printf("===============%s(%s)===============\n", title, tree->text);
	printf("     Initial sibling count is   %d\n",  SiblingCount(tree));
	printf("     Total Node Count           %d\n", TotalNodeCount(tree));
	printf("     Total Size(Resursive)      %d\n", TotalSize(tree));
	printf("     Child Count                %d\n", ChildCount(tree, false));
	printf("     Total Size At this level:  %d\n", Size(tree));
	printf("     Visible Size At this level:  %d\n", VisibleSize(tree));
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


	comment_item_tree* root = make_sample_tree();

	int _raysz = 10 * 1000;

	ND** array = allocNodeArray(_raysz);
	array[0] = root;

	printf("Done assigning to flat array...\n");

	int _x = 0;
	for(_x = 0;_x < _raysz;_x++){
		printf("Array Element %d is %p\n", _x, array[_x]);
	}


	return 0;

	if (SearchTree(root, "Third Level")) {
		printf("Search Succeded\n");
	}else{
		printf("Search Failed\n");
	}
	PrintTree(root, PRINT_ALL_TREE);
	printf("----------------printing only the subtre------------------\n");
	PrintTree(root, PRINT_ONLY_EXPANDED_NODES);
	SetSingleExpansionState(root, FALSE);
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
	SetSingleExpansionState(root, TRUE);
	PrintTree(root, PRINT_ALL_TREE);
	printf("----------------set expansion state to false--------------\n");
	SetSingleExpansionState(root, FALSE);
	dump(root, "After Everything");
	SetSingleExpansionState(root, TRUE);
	dump(root, "After Expanding root");
	int sizeTotal = TotalSize(root);
	printf("Total size is %d\n", sizeTotal);
	PrintTree(root, PRINT_ALL_TREE);
	printf("---------------------------------------\n");
	PrintTree(root, PRINT_ONLY_EXPANDED_NODES);
	//assert(SiblingCount(root)   == 0);
	//assert(TotalSize(root) == 11);

	printf("Extracting to flat tree\n");
	int sz = -1;
	comment_item_tree** _flat = ToFlatTree(root, &sz);
        
	printf("Extracted to flat tree - sz was %d and tree was %p\n", sz, _flat);
	int PRINT_WIDTH=20;
        int y = 0;
        printf("Position %*s  %*s %*s  \n", PRINT_WIDTH, "This", PRINT_WIDTH, "Parent", PRINT_WIDTH, "Previous");
        for(y = 0;y < sz;y++){
            printf("%7d   %20p %20p %20p %4d is %*s\n", y, _flat[y], _flat[y]->parent, _flat[y]->previous, ComputedDepth(_flat[y]), 10 + (2 * _flat[y]->_ft_depth), _flat[y]->text );
        }
	SetExpansionState(root, FALSE);
	SetExpansionState(root, TRUE);
        free(_flat);
	_flat = ToFlatTree(root, &sz);
        
	printf("Extracted to full flat tree - sz was %d and tree was %p\n", sz, _flat);
            printf("Position %*s  %*s %*s  \n", PRINT_WIDTH, "This", PRINT_WIDTH, "Parent", PRINT_WIDTH, "Previous");
        for(y = 0;y < sz;y++){
            printf("%7d   %20p %20p %20p %4d is %*s\n", y, _flat[y], _flat[y]->parent, _flat[y]->previous, ComputedDepth(_flat[y]), 10 + (2 * _flat[y]->_ft_depth), _flat[y]->text );
        }

	return 0;
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
		if (had_newline == true) {
			while (_foo[start] == ' ') {
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
	printf("Tree Depth %d \n", TreeMaxDepth(root));


	return 0;
}
