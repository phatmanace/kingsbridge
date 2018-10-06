#define _GNU_SOURCE 
#include <ncurses.h>
#include <stdio.h>
#include "queue.h"
#include "comment_tree.h"
#include "comment_fetch.h"
#include "t_sample.h"
#include "zlog.h"
#include <dirent.h>
#include <pthread.h>
#include <string.h>
#include "curl/curl.h"
#include <stdlib.h>
#include <unistd.h>
#include "downloader.h"
#define NUMT 1
#define _TS_CURSES_DEBUG 1
#define MODE_NORMAL 0
#define MODE_LOADING 1
#define SMALL_HN_ARTICLE 18154825
#define BIG_HN_ARTICLE 18063893
#define BAD_HN_ARTICLE 18063893
#define HN_ARTICLE SMALL_HN_ARTICLE
#define MAX(x, y) ((x) < (y) ? (y) : (x))
#define MIN(x, y) ((x) > (y) ? (y) : (x))


zlog_category_t *c;
int _selected = -1;
comment_item_tree** _flat = NULL;
int _total_tree_size = 0;
int WINDOW_SIZE = 26;
int start_row = 0;
char *msg1 = NULL;
int mode = MODE_NORMAL;
WINDOW* win;

pthread_mutex_t lock;

comment_item_tree* tree = NULL;
WINDOW *create_newwin(int height, int width, int starty, int startx);
void destroy_win(WINDOW *local_win);
void RenderTreeIntoWindow(comment_item_tree* tree);
void moveDown(WINDOW* win);
void moveUp(WINDOW* win);
void toggleExpand(WINDOW* win, bool newState);

typedef struct{
	int startx, starty, width, height;
}window_info;

void DrawingCallback(int level, char* format, ...){

	#ifdef _TS_CURSES_DEBUG 
	va_list args;
	va_start(args, format);
	char* buffer;
	vasprintf(&buffer, format, args);
	va_end(args);
	zlog_info(c, "Curses_tester: %s", buffer);
	if(level > 1){
		if(msg1 != NULL){
			free(msg1);
		}
		msg1 = malloc(strlen(buffer) + 1);
		strcpy(msg1, buffer);
		free(buffer);
		RenderTreeIntoWindow( tree);
		refresh();
	}
	wrefresh(win);
	#endif
}

void *downloadSingleURL(void *x)
{
	struct thread_args* args = (struct thread_args*)x;
	zlog_info(c, "Downloading Single URL");
	if(args->callback == NULL){
		zlog_info(c, "Abort - callback function was null\n");
		return (void*)0;
	}


	unsigned int self = (unsigned int)pthread_self();

	args->callback(1, "[%u] In Thread loop\n", self);
	CURL *curl;
	curl = curl_easy_init();
	if (!curl) {
		args->callback(1, "Exiting... Curl didn't seem to init correctly\n");
		return (void*)0;
	}
	args->callback(1, "[After init] In Thread loop\n");
	if (args->queue == NULL) {
		args->callback(1, "Queue was null...");
	}
	;
	args->callback(1, "[%u] Starting up thread while()..\n", self);
	int tries = 3;
	while (tries > 0) {
		while (QSize(args->queue) > 0) {
			if(tries == 1){
				tries++;
			}
				downloadURL(lock, args, self, curl);

			int _sz = QSize(args->queue);
			args->callback(1, "[Thread %u] Computing size ", self);
			args->callback(1, "[Thread %u] Iteration Complete [%d] ", _sz, self);
			//free(response_string.ptr);
		}
		tries--;
		sleep(1);
		args->callback(1, "[%u] Loop end... tries are now %d", self, tries);
	}
	args->callback(1, "[Thread %u] Complete and exiting (errors=%d)... ", self, args->error_code);
	curl_easy_cleanup(curl);
	return (void*)0;

}

void RenderRow(WINDOW* win, ND* node, int* row, int basecolumn, int rowindex,  int _y, int mode)
{

	int column = basecolumn + (2 * node->_ft_depth);

	if (node == NULL) {
		zlog_info(c, "Null Render (mode=%d, rowindex=%d)", mode, rowindex);
		return;
	}
	/*
	zlog_info(c, "[%d/%d]    Real_rende_call() %s at row %d/%d = col %d/%d",
		  rowindex, mode,  node->text,
		  *row, isExpanded(node), column, basecolumn);
	*/
	if (node->children != NULL && isExpanded(node)) {
		mvwhline(win, *row, column -  1, ACS_HLINE, 2 );
		mvwhline(win, *row, column - 1, ACS_URCORNER, 1 );

	}else{
	}
	int _fkd = column + 1;
	/*
	 * Draw out all the lines back for the parent nodes
	 */
	if (rowindex == _selected) {
		wattron(win, COLOR_PAIR(4));
	}
	if (node->_ft_depth > 0) {
		while (true ) {
			_fkd--;
			_fkd--;
			if (_fkd <= basecolumn) {
				break;
			}
			mvwvline(win, (*row),  _fkd, ACS_VLINE, 1 );
			//zlog_info(c, "         Working back, col now %d, %d, %d", node->_ft_depth,  _fkd, column);
		}
	}
	if (rowindex == _selected) {
		wattron(win, COLOR_PAIR(3));
	}

	/* Actually render the node details.
	 */

	// Clear the row...
	wmove(win, *row, column + 3);  wprintw(win, "                                                       ");
	// Draw the spine line to the text
	int SPLINE_COL = 4;
	mvwhline(win, *row, column - 1, ACS_HLINE, SPLINE_COL );
	if (node->next != NULL) {
		wmove(win, *row, column - 1 );  waddch(win, ACS_LTEE);
	}else{
		wmove(win, *row, column - 1 );  waddch(win, ACS_LLCORNER);
	}

	wmove(win, *row, basecolumn - 3);  wprintw(win, "%d", *row);
	// Actually draw the row text 
	if (node->children != NULL && !isExpanded(node)) {
		wmove(win, *row, column + SPLINE_COL);  wprintw(win, "[%d|%d] %s",  TotalSize(node->children), ChildCount(node, SHALLOW_DEPTH), substring(node->text, 60));
	}else{
		wmove(win, *row, column + SPLINE_COL);  wprintw(win, substring(node->text, 60));
	}
	wattron(win, COLOR_PAIR(3));
	wmove(win, *row, 85); wprintw(win, "cc=%3d ccT=%3d d=%2d ",
				      ChildCount(node, SHALLOW_DEPTH),
				      ChildCount(node, DEEP_DEPTH),
				      node->_ft_depth
				      );
	wattroff(win, COLOR_PAIR(3));
	// deactivate colors
	if (rowindex == _selected) {
		wattroff(win, COLOR_PAIR(2));
	}

	//zlog_info(c, "RenderRow(): Finish main render loop");
	(*row)++;


	// Draw in window information 

	wattron(win, COLOR_PAIR(3));
	wmove(win, 2, 55); wprintw(win, "cc=%3d tcc=%3d d=%4d TTS=%4d SEL=%3d SR=%d R=%3d, NB=%3d B=%d, WS=%d",
				      ChildCount(node, SHALLOW_DEPTH),
				      ChildCount(node, DEEP_DEPTH),
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
}

void DrawBox(WINDOW* win, int height, int width, int starty, int startx, bool isFirst, bool isLast)
{
	mvwhline(win, starty, startx + 1, ACS_HLINE, width  );
	mvwhline(win, starty + height - 1, startx + 1, ACS_HLINE, width );
	mvwvline(win, starty + 1, startx, ACS_VLINE, height - 2 );
	//mvwvline(win, starty + 1 , width + startx, ACS_VLINE, height - 2 );
	zlog_info(c, "Drawing box (h=%3d, w=%3d, starty=%3d, startx=%3d, f=%d, l=%d",
		  height,
		  width,
		  starty,
		  startx,
		  isFirst,
		  isLast);

	wmove(win, starty + 1, startx + 2);  wprintw(win, "%d,%d,%d", startx, starty, (starty % 3));


	if (isFirst) {
		wmove(win, starty, startx); waddch(win, ACS_ULCORNER);
		wmove(win, starty + height - 1, startx); waddch(win, ACS_LLCORNER);
	}
	if (!isLast) {
		wmove(win, starty, startx + width); waddch(win, ACS_TTEE);
		wmove(win, starty + height - 1, startx + width); waddch(win, ACS_BTEE);
	}else{
		wmove(win, starty, startx + width); waddch(win, ACS_URCORNER);
		wmove(win, starty + height - 1, startx + width); waddch(win, ACS_LRCORNER);
		mvwvline(win, starty + 1, width + startx, ACS_VLINE, height - 2 );

	}


}

void RenderTreeIntoWindow(comment_item_tree* tree)
{
	zlog_info(c, "Rendering window %dX%d", win->_maxy, win->_maxx);
	wbkgd(win, COLOR_PAIR(1));
	refresh();
	wmove(win, 1, 1);
	//wattron(win, A_BOLD | A_BLINK);
	wattron(win,  A_BOLD);
	wattron(win, COLOR_PAIR(2));
	zlog_info(c, "Starting to parse the tree");
	if(mode == MODE_LOADING){
		wprintw(win, "Loading in articles...one moment please ............ ");
	}else{
		wprintw(win, "(%d/%d) - News Comments for article ( %d in total) ", WINDOW_SIZE, LINES, TotalSize(tree) );
	}
	wmove(win, 0,  COLS - 15 );
	wprintw(win, "[%d-%d]", COLS, WINDOW_SIZE);
	wattroff(win, COLOR_PAIR(2));
	wattroff(win, A_BOLD);

	int row = 4;
	while (row < 30) {
		wmove(win, row, 1);  wprintw(win, "%*s", 120, "");
		row++;
	}
	if(msg1 != NULL && mode == MODE_LOADING){
		wmove(win, LINES - 4, 1);
		wprintw(win, "%s", msg1);
	}


	/* What does 4 mean?
	 */
	row = 4;
	int txcol = 7;
	if(_flat != NULL){
		free(_flat);
	}
	ResetFlatTree(tree);
	_flat = ToFlatTree(tree, &_total_tree_size);

	zlog_info(c, "Rendering total of  %d rows", _total_tree_size);

	/* Render entire Tree into Window
	 * Idea is to stop at the end of the display panel
	 * Or the end of the article list, which ever we hit sooner
	 */

	while (true) {
		if ( (row - 4) == _total_tree_size || (row) >= WINDOW_SIZE) {
			break;
		}
		int chosen_element = MAX(row + start_row - 4, 0);
		if(chosen_element >= _total_tree_size){
			zlog_info(c, "Hit Max, stopping iteration..");
			break;
		}
		//zlog_info(c, ">>Rendering  row  %d, ray elem %d - real elem/index %d ", row, row + start_row - 4, chosen_element);
		RenderRow(win, _flat[chosen_element], &row, txcol, row - 4, 0, -1);
	}


	// Horizontal Dividing line between tree and Text.. 
	mvwhline(win,  WINDOW_SIZE ,  1, ACS_HLINE, COLS  );
	wmove(win,  WINDOW_SIZE , COLS - 3); waddch(win, ACS_RTEE);

	int _text_start = (int)(LINES/2) + 2;

	/* If the user has selected an article, then
	 * Actually Render the text that we want to see on the window
	 */

	if(_selected > -1){
		row = _text_start;
		while (row < LINES - 2) {
			wmove(win, row, 2);  wprintw(win, "%*s", 120, "");
			row++;
		}

		wmove(win, _text_start, 1);
		s_segments* segs = splitIntoSegments(_flat[_selected]->text, 90);
		int _p = 0;
		for (_p = 0; _p < segs->count; _p++) {
			wmove(win, _text_start + _p, 4);
			wprintw(win, "%s", segs->segments[_p]->string);
		}
		freeSegs(segs);
		if(msg1 != NULL){
			free(msg1);
		}
		msg1 = malloc(50);
		memset(msg1, 0, 50);
		sprintf(msg1, "ID: %d", _flat[_selected]->id);
		wmove(win, LINES - 4, 1);
		wprintw(win, "%120s", "");
		wmove(win, LINES - 4, 1);
		wprintw(win, "%s", msg1);
	}


}


ND* BuildTree()
{


	//int id = 11364550;
	int id = HN_ARTICLE;

	curl_global_init(CURL_GLOBAL_ALL);
	pthread_t thread[NUMT];
	pthread_mutex_init(&lock, NULL);
	QH* queue = newQueue();
	int article_count = 0;
	struct thread_args* args = malloc(sizeof(struct thread_args));;
	args->count = &article_count;
	args->queue = queue;
	args->callback = &DrawingCallback;
	args->tcount = 0;
	args->error_code = 0;
	args->noderay = allocNodeArray(1000);
	args->last_pushed_elem = 0;
	//QAppendItem(queue, 11314597 , &lock);

	QAppendItem(queue, id, &lock);
	//PrintQueue(queue);
	//return 0 ;
	struct timeval tv1, tv2;
	gettimeofday(&tv1, NULL);

	int i = 0;
	for (i = 0; i < NUMT; i++) {
		args->threadid = i;
		pthread_create(&thread[i], NULL, downloadSingleURL, args);
	}
	for (i = 0; i < NUMT; i++) {
		pthread_join(thread[i], NULL);
	}
	pthread_mutex_destroy(&lock);
	QueueEntireClear(queue);
	free(queue);
	curl_global_cleanup();
	gettimeofday(&tv2, NULL);
	double time_spent = (double)(tv2.tv_usec - tv1.tv_usec) / 1000000 +
			    (double)(tv2.tv_sec - tv1.tv_sec);
	zlog_info(c, "Downloaded articles in %lf Seconds", time_spent);

	ND* root = nCmmtText( "Article Header would go here", id);
	buildCommentTree(root, args->noderay, 1000, 0);
	free(args);
	return root;
}



void RefreshData(){
	mode = MODE_LOADING;
	RenderTreeIntoWindow(tree);
	refresh();
	wrefresh(win);
	tree = BuildTree();
	mode = MODE_NORMAL;
	RenderTreeIntoWindow(tree);
	refresh();
	wrefresh(win);
}

void ResetWindowInfo(window_info* info){
	info->height = LINES - 2;
	info->width  = COLS - 2;
	info->starty = (LINES - info->height) / 4;  /* Calculating for a center placement */
	info->startx = (COLS - info->width) / 2;    /* of */
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
	if (!c) {
		printf("Logging init (category) fail\n");
		return -1;
	}




	zlog_info(c, "Program starting... ");

	zlog_info(c, "Created Tree From Sample Text..");
	SetExpansionState(tree, TRUE);
	LogPrintTree(c, tree, PRINT_ONLY_EXPANDED_NODES);
	zlog_info(c, "Tree Size: %d\n", TotalSize(tree));
	initscr();

	WINDOW_SIZE=(int)(LINES/2);

	mousemask(ALL_MOUSE_EVENTS, NULL);
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
	init_pair(4, COLOR_RED, COLOR_BLUE);

	window_info w_info;
	ResetWindowInfo(&w_info);
	
	refresh();
	win = create_newwin(w_info.height, w_info.width, w_info.starty, w_info.startx);
	if (win == NULL) {
		zlog_info(c, "Window Was null...\n");
		endwin();
		printf("Window was null??\n");
		return -1;
	}

	zlog_info(c, "Rendering Tree into window %p \n", win);
	RenderTreeIntoWindow(tree);
	refresh();
	wrefresh(win);

	int ch;
	while ((ch = getch()) != KEY_F(4)) {
		//zlog_info(c, "Trapped Keypress  %d", ch );
		MEVENT event;
		switch (ch) {
		case KEY_RESIZE:
			zlog_info(c, "Resize()");
			destroy_win(win);
			ResetWindowInfo(&w_info);
			win = create_newwin(w_info.height, w_info.width, w_info.starty, w_info.startx);
			RenderTreeIntoWindow(tree);
			refresh();
			wrefresh(win);
			break;
		case KEY_F(3):
			zlog_info(c, "F3=> Refresh");
			RefreshData();
			break;
		case KEY_LEFT:
			toggleExpand(win, false);
			break;
		case KEY_RIGHT:
			toggleExpand(win, true);
			break;
		case KEY_UP:
			moveUp(win);
			break;
		case KEY_DOWN:
			moveDown(win);
			break;

		}
		if (getmouse(&event) == OK) {
			zlog_info(c, "Mouse => %d, %d", event.x, event.y);
		}
		//LogPrintTree(c, tree, PRINT_ALL_TREE);

	}

	endwin();
	return 0;
}

void toggleExpand(WINDOW* win, bool newState)
{
	if(_selected > -1){
		SetSingleExpansionState(_flat[_selected], newState);
		RenderTreeIntoWindow( tree);
		wrefresh(win);
		zlog_info(c, "Expansion state of of %p is %d\n", _flat[_selected], isExpanded(_flat[_selected]));
	}
	//LogPrintTree(c, tree, PRINT_ONLY_EXPANDED_NODES);

}
void moveDown(WINDOW* win)
{
	zlog_info(c, "[MoveDown()] is selected %d , _total= %d WS=%d", _selected, _total_tree_size,WINDOW_SIZE);
	if (_selected - start_row >= WINDOW_SIZE - 1) {
		zlog_info(c, "Scrolling off bottom of screen");
		start_row++;
		_selected++;
		RenderTreeIntoWindow( tree);
		wrefresh(win);
		return;

	}
	if (_selected < (_total_tree_size - 1)) {
		zlog_info(c, "nudging down a row");
		_selected++;
		RenderTreeIntoWindow( tree);
		wrefresh(win);
	}else{
		zlog_info(c, "At bottom");
		start_row++;
		RenderTreeIntoWindow( tree);
		wrefresh(win);
	}
}
void moveUp(WINDOW* win)
{
	zlog_info(c, "MoveUp() - is selected %d > %d\n", _selected, start_row);
	if (_selected == 0) {
		zlog_info(c, "MoveUp() At Top");
		return;
	}
	if (_selected - start_row <= 0 ) {
		start_row--;
		_selected--;
		RenderTreeIntoWindow( tree);
		wrefresh(win);
		return;

	}
	if (_selected > 0) {
		_selected--;
		RenderTreeIntoWindow( tree);
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

void destroy_win(WINDOW *local_win)
{	
	/* box(local_win, ' ', ' '); : This won't produce the desired
	 * result of erasing the window. It will leave it's four corners 
	 * and so an ugly remnant of window. 
	 */
	zlog_info(c, "Destroying Window...");
	wborder(local_win, ' ', ' ', ' ',' ',' ',' ',' ',' ');
	/* The parameters taken are 
	 * 1. win: the window on which to operate
	 * 2. ls: character to be used for the left side of the window 
	 * 3. rs: character to be used for the right side of the window 
	 * 4. ts: character to be used for the top side of the window 
	 * 5. bs: character to be used for the bottom side of the window 
	 * 6. tl: character to be used for the top left corner of the window 
	 * 7. tr: character to be used for the top right corner of the window 
	 * 8. bl: character to be used for the bottom left corner of the window 
	 * 9. br: character to be used for the bottom right corner of the window
	 */
	wrefresh(local_win);
	delwin(local_win);
}
