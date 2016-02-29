#include <ncurses.h>
#include "comment_tree.h"
#include "t_sample.h"
#include "zlog.h"

zlog_category_t *c;
comment_item_tree* _selected = NULL;
comment_item_tree* tree = NULL;
WINDOW *create_newwin(int height, int width, int starty, int startx);
void moveDown(WINDOW* win);
void toggleExpand(WINDOW* win, bool newState);


void RenderRow(WINDOW* win, comment_item_tree* node, int* row, int column, int depth,  int _y, int mode)
{

	if (node == NULL) {
		zlog_info(c, "Null Render (mode=%d, depth=%d)", mode, depth);
		return;
	}
	(*row)++;
	zlog_info(c, "[%d/%d]    Real_rende_call() %s at row %d/%d", depth, mode,  node->text, *row, isExpanded(node));
	if (node->children != NULL && isExpanded(node)) {
		mvwhline(win, *row, column -  2, ACS_HLINE, 2 );
		if (node->next != NULL) {
			mvwvline(win, *row, column -  3, ACS_LTEE, 1 );
		}else{
			mvwvline(win, *row, column -  3, ACS_LLCORNER, 1 );
		}
	}else{
		mvwhline(win, *row, column -  2, ACS_PLUS, 1 );
	}
	if (node == _selected) {
		wattron(win, COLOR_PAIR(2));
	}
	/* Actually render the node details.
	 */
	wmove(win, *row, column + 3);  wprintw(win, node->text);
	wmove(win, *row, column + 23); wprintw(win, "%d %p %d", ChildCount(node, false),  node, _y);
	// deactivate colors
	if (node == _selected) {
		wattroff(win, COLOR_PAIR(2));
	}

	/*
	 * Deal with child elements
	 */
	if (node->children != NULL && isExpanded(node)) {
		RenderRow(win, node->children, row, column + 1, depth + 1, _y, 2);
	}
	comment_item_tree* nextSibling = node->next;
	int _i = 0;
	while (nextSibling != NULL) {
		_i++;
		zlog_info(c, "[%d %d m:%d] Rendering next() %s/%p at row %d/%d", depth, _i, mode,  nextSibling->text, nextSibling, *row, isExpanded(nextSibling));
		RenderRow(win, nextSibling, row, column, depth, _i, 1);
		zlog_info(c, "nextSibling was %p->%p", nextSibling, nextSibling->next);
		nextSibling = nextSibling->next;
	}
	zlog_info(c, "Leaving..");
}

void RenderTreeIntoWindow(WINDOW* win, comment_item_tree* tree)
{
	zlog_info(c, "Rendering window %dX%d", win->_maxy, win->_maxx);

	int row = 4;
	int txcol = 7;
	comment_item_tree* tmp = tree;
	while (tmp != NULL) {
		RenderRow(win, tmp, &row, txcol, 0, 0, -1);
		tmp = tmp->next;
	}

}



int main(void)
{
	int l_init = 0;

	l_init = zlog_init("/etc/zlog.conf");
	if (l_init) {
		printf("logging init failed");
		return -1;
	}
	c = zlog_get_category("hngui");

	zlog_info(c, "Program starting... ");
	tree = make_sample_tree();
	_selected = tree;
	LogPrintTree(c, tree, PRINT_ONLY_EXPANDED_NODES);
	zlog_info(c, "Tree Size: %d\n", TotalSize(tree));
	initscr();
	cbreak();
	noecho();
	nonl();
	curs_set(0);
	keypad(stdscr, TRUE);
	start_color();
	curs_set(0);
	init_pair(1, COLOR_YELLOW, COLOR_BLACK);
	init_pair(2, COLOR_RED, COLOR_BLACK);
	init_pair(3, COLOR_YELLOW, COLOR_BLUE);

	int startx, starty, width, height;
	height = 80;
	width  = 100;
	starty = (LINES - height) / 4;  /* Calculating for a center placement */
	startx = (COLS - width) / 2;    /* of */
	refresh();
	WINDOW *win = create_newwin(height, width, starty, startx);
	if (win == NULL) {
		zlog_info(c, "Window Was null...\n");
		endwin();
		return -1;
	}
	wbkgd(win, COLOR_PAIR(1));
	refresh();
	wmove(win, 2, 1);
	wattron(win, A_BOLD | A_BLINK);
	wattron(win, COLOR_PAIR(2));
	zlog_info(c, "Starting to parse the tree");
	wprintw(win, "News Comments for ... %d ", TotalSize(tree) );
	wattroff(win, COLOR_PAIR(2));

	wattroff(win, A_BOLD | A_BLINK);


	zlog_info(c, "Rendering Tree into window %p \n", win);
	RenderTreeIntoWindow(win, tree);
	refresh();
	wrefresh(win);

	int ch;
	while ((ch = getch()) != KEY_F(4)) {
		zlog_info(c, "Trapped Keypress  %d", ch );
		switch (ch) {
		case KEY_LEFT:
			zlog_info(c, "Left key");
			toggleExpand(win, false);
			break;
		case KEY_RIGHT:
			zlog_info(c, "Right key");
			toggleExpand(win, true);
			break;
		case KEY_UP:
			zlog_info(c, "Up Key");
			break;
		case KEY_DOWN:
			zlog_info(c, "Down Key");
			moveDown(win);
			break;

		}
		LogPrintTree(c, tree, PRINT_ALL_TREE);

	}

	endwin();
	return 0;
}

void toggleExpand(WINDOW* win, bool newState)
{
	SetSingleExpansionState(_selected, newState);
	RenderTreeIntoWindow(win, tree);
	wrefresh(win);
	zlog_info(c, "Expansion state of of %p is %d\n", _selected, isExpanded(_selected));
	LogPrintTree(c, tree, PRINT_ONLY_EXPANDED_NODES);

}
void moveDown(WINDOW* win)
{
	zlog_info(c, "Checking children of %p\n", _selected);
	if (GetNextTreeElement(_selected) != NULL) {
		_selected = GetNextTreeElement(_selected);
		RenderTreeIntoWindow(win, tree);
		wrefresh(win);
	}else{
		zlog_info(c, "At bottom");
	}
}

WINDOW *create_newwin(int height, int width, int starty, int startx)
{
	WINDOW *local_win;

	local_win = newwin(height, width, starty, startx);
	box(local_win, ACS_VLINE, ACS_HLINE);   /* 0, 0 gives default characters
	                                         * for the vertical and horizontal
	                                         *                                       * lines			*/
	wrefresh(local_win);                    /* Show that box                */

	return local_win;
}
