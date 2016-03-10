#include <ncurses.h>
#include "comment_tree.h"
#include "t_sample.h"
#include "zlog.h"
#define MAX(x,y) ((x) < (y) ? (y) : (x))
#define MIN(x,y) ((x) > (y) ? (y) : (x))


zlog_category_t *c;
int _selected = -1;
comment_item_tree** _flat = NULL;
int _total_tree_size = 0;
int WINDOW_SIZE = 16;
int start_row = 0;

comment_item_tree* tree = NULL;
WINDOW *create_newwin(int height, int width, int starty, int startx);
void moveDown(WINDOW* win);
void moveUp(WINDOW* win);
void toggleExpand(WINDOW* win, bool newState);


void RenderRow(WINDOW* win, comment_item_tree* node, int* row, int basecolumn, int rowindex,  int _y, int mode)
{

	int column = basecolumn + node->_ft_depth;
	if (node == NULL) {
		zlog_info(c, "Null Render (mode=%d, rowindex=%d)", mode, rowindex);
		return;
	}
	zlog_info(c, "[%d/%d]    Real_rende_call() %s at row %d/%d = col %d/%d", rowindex, mode,  node->text, *row, isExpanded(node), column, basecolumn);
	if (node->children != NULL && isExpanded(node)) {
		mvwhline(win, *row, column -  2, ACS_HLINE, 2 );
		mvwhline(win, *row, column -1  , ACS_URCORNER, 1 );
		if(rowindex > 0){
			mvwvline(win, (*row) - 1, column -  2, ACS_VLINE, 2 );
		}
	}else{
		if(node->children != NULL){
			mvwhline(win, *row, column -  2, ACS_PLUS, 1 );
		}else{
			mvwhline(win, *row, column -  2, ACS_VLINE, 1 );
		}
	}
	wattron(win, A_DIM );
	int _fkd = node->_ft_depth;
	while(_fkd-- > 2 ){
		mvwvline(win, (*row) - 1, basecolumn + _fkd -  2, ACS_VLINE, 2 );
	}
	wattroff(win, A_DIM );

	if (rowindex == _selected) {
		wattron(win, COLOR_PAIR(2));
	}
	/* Actually render the node details.
	 */

	wmove(win, *row, column + 3);  wprintw(win, "                                                       ");
	mvwhline(win, *row, column - 1 , ACS_HLINE, 3 );
	wmove(win, *row, column - 1 );  waddch(win, ACS_LTEE);
	if(node->next == NULL && node->children == NULL){
		wmove(win, *row, column - 1 );  waddch(win, ACS_LLCORNER);
	}
	if(node->children != NULL && isExpanded(node)){
		wmove(win, *row, column - 0 );  waddch(win, ACS_URCORNER);
	}
	if(node->children != NULL && !isExpanded(node)){
		wmove(win, *row, column + 3);  wprintw(win, "%s (%d)", node->text, TotalSize(node->children));
	}else{
		wmove(win, *row, column + 3);  wprintw(win, node->text);
		if(node->id % 2 == 0){
			//(*row)++;
			//wmove(win, *row, column + 3);  wprintw(win, "A Second Row of text");
			//mvwvline(win, *row - 1, column + 1 , ACS_VLINE, 2 );
			//_fkd = node->_ft_depth;
			//while(_fkd-- > 2 ){
			//	mvwvline(win, (*row) - 1, basecolumn + _fkd -  2, ACS_VLINE, 2 );
			//}
		}
	}
	wattron(win, COLOR_PAIR(3));
	wmove(win, *row, 35); wprintw(win, "cc=%4d  d=%4d TTS=%4d SEL=%3d SR=%d R=%3d, NB=%3d B=%d, WS=%d",
		       								ChildCount(node, false),  
										node->_ft_depth, 
										_total_tree_size, 
										_selected, 
										start_row, 
										*row, 
										_selected - start_row, 
										_selected - start_row > WINDOW_SIZE, 
										WINDOW_SIZE
										
										);
	wattroff(win, COLOR_PAIR(3));
	// deactivate colors
	if (rowindex == _selected) {
		wattroff(win, COLOR_PAIR(2));
	}

	zlog_info(c, "Done Rendering Leaving..");
	(*row)++;
}

void DrawBox(WINDOW* win, int height, int width, int starty, int startx, bool isFirst, bool isLast)
{
	mvwhline(win, starty, startx + 1, ACS_HLINE, width  );
	mvwhline(win, starty + height - 1, startx + 1, ACS_HLINE, width );
	mvwvline(win, starty + 1, startx, ACS_VLINE, height - 2 );
	//mvwvline(win, starty + 1 , width + startx, ACS_VLINE, height - 2 );

	if(isFirst){
		wmove(win, starty, startx); waddch(win,ACS_ULCORNER);
		wmove(win, starty + height - 1, startx); waddch(win,ACS_LLCORNER);
	}else{
		wmove(win, starty, startx + width); waddch(win,ACS_TTEE);
		wmove(win, starty + height - 1, startx + width); waddch(win,ACS_BTEE);
	}

}

void RenderTreeIntoWindow(WINDOW* win, comment_item_tree* tree)
{
	zlog_info(c, "Rendering window %dX%d", win->_maxy, win->_maxx);

	int row = 4;
	while(row < 30){	
		wmove(win, row, 1);  wprintw(win, "%*s", 120, "");
		row++;
	}

	row = 4; 
	int txcol = 7;
	_flat = ToFlatTree(tree, &_total_tree_size);

	zlog_info(c, "Rendering total of  %d rows", _total_tree_size);
	
	while(true){
		if( (row - 4) == _total_tree_size || (row - 4) >= WINDOW_SIZE){
			break;
		}
		int chosen_element = MAX(row + start_row - 4, 0);
		zlog_info(c, ">>Rendering  row  %d, ray elem %d - real elem %d ", row, row + start_row - 4, chosen_element);
		RenderRow(win, _flat[chosen_element], &row, txcol , row - 4, 0, -1);
	}


	int BOX_W = 9;
	int BOX_H = 3;
	int _stx = 1;
	int _sty = 21;
		

	while(_sty < 35){
		while(_stx < 80){
			//wattron(win, A_BOLD | A_BLINK);
			DrawBox(win, BOX_H, BOX_W,  _sty, _stx, _stx == 1, _stx > 68);
			//wattroff(win, A_BOLD | A_BLINK);
			_stx += BOX_W ;
		}
		_sty += BOX_H ;
		_stx = 1;
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
	if(!c){
		printf("Logging init (category) fail\n");
		return -1;
	}

	zlog_info(c, "Program starting... ");
	tree = make_sample_tree();
	SetExpansionState(tree, TRUE);
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
	height = 40;
	width  = 150;
	starty = (LINES - height) / 4;  /* Calculating for a center placement */
	startx = (COLS - width) / 2;    /* of */
	refresh();
	WINDOW *win = create_newwin(height, width, starty, startx);
	if (win == NULL) {
		zlog_info(c, "Window Was null...\n");
		endwin();
		printf("Window was null??\n");
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
			moveUp(win);
			break;
		case KEY_DOWN:
			zlog_info(c, "Down Key");
			moveDown(win);
			break;

		}
		//LogPrintTree(c, tree, PRINT_ALL_TREE);

	}

	endwin();
	return 0;
}

void toggleExpand(WINDOW* win, bool newState)
{
	SetSingleExpansionState(_flat[_selected], newState);
	RenderTreeIntoWindow(win, tree);
	wrefresh(win);
	zlog_info(c, "Expansion state of of %p is %d\n", _flat[_selected], isExpanded(_flat[_selected]));
	//LogPrintTree(c, tree, PRINT_ONLY_EXPANDED_NODES);

}
void moveDown(WINDOW* win)
{
	zlog_info(c, "is selected %d > %d\n", _selected, _total_tree_size);
	if(_selected - start_row >= WINDOW_SIZE - 1){
		start_row++;
		_selected++;
		RenderTreeIntoWindow(win, tree);
		wrefresh(win);
		return;

	}
	if (_selected < (_total_tree_size - 1)){
		_selected++;
		RenderTreeIntoWindow(win, tree);
		wrefresh(win);
	}else{
		start_row++;
		RenderTreeIntoWindow(win, tree);
		wrefresh(win);
		zlog_info(c, "At bottom");
	}
}
void moveUp(WINDOW* win)
{
	zlog_info(c, "MoveUp() - is selected %d > %d\n", _selected, start_row);
	if(_selected == 0){
		zlog_info(c, "MoveUp() At Top");
		return;
	}
	if(_selected - start_row <= 0 ){
		start_row--;
		_selected--;
		RenderTreeIntoWindow(win, tree);
		wrefresh(win);
		return;

	}
	if (_selected > 0){
		_selected--;
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
