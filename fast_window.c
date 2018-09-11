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
#define BAD_HN_ARTICLE 11569726
#define HN_ARTICLE 11569857
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
void moveDown(WINDOW* win);
void moveUp(WINDOW* win);
void toggleExpand(WINDOW* win, bool newState);

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

	wmove(win, starty + 1, startx + 2);  //wprintw(win, "%d,%d,%d", startx, starty, (starty % 3));

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



void RefreshData(){
	mode = MODE_LOADING;
	refresh();
	wrefresh(win);
	mode = MODE_NORMAL;
	refresh();
	
	int h = 40;
	int w = 35;

	DrawBox(win, h, w,1,1,true, true);
	wattron(win, COLOR_PAIR(1));
	DrawBox(win, h, w,1,2 + w,true, true);
	wattron(win, COLOR_PAIR(2));
	DrawBox(win, h, w,1,3 + (2 * w),true, true);
	wrefresh(win);
}

void *barrelShiftArray(void* _strings){
	char** strings = (char**)_strings;

	for(int x = 0; x < 1000;x++){
		usleep(1000000);
		for(int i = 0;i < 100;i++){
			sprintf(strings[i], "not_a_test %d", x);
		}
		zlog_info(c, "Sleeping... ");
	}

	return NULL;
}


int main(void)
{
	int l_init = 0;

	l_init = zlog_init("/etc/zlog.conf");
	if (l_init) {
		printf("logging init failed\n");
		return -1;
	}
	c = zlog_get_category("hngui");
	if (!c) {
		printf("Logging init (category) fail\n");
		return -1;
	}

	pthread_t thread;



	char** strings = malloc(sizeof(char*) * 100);

	for(int x = 0;x < 100;x++){
		strings[x] = malloc(sizeof(char) * 10);
		sprintf(strings[x], "test %d", x);
	}
	pthread_create(&thread, NULL, barrelShiftArray, strings);


	for(int x = 0;x < 100;x++){ zlog_info(c, "%s", strings[x]);}

	for(int x = 0;x < 100;x++){
		sprintf(strings[x], "not_a_test %d", x);
	}

	for(int x = 0;x < 100;x++){ zlog_info(c, "%s", strings[x]);}


	zlog_info(c, "Program starting... ");


	zlog_info(c, "Sample Fast Window App");
	SetExpansionState(tree, TRUE);
	LogPrintTree(c, tree, PRINT_ONLY_EXPANDED_NODES);
	zlog_info(c, "Tree Size: %d\n", TotalSize(tree));
	initscr();
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

	int startx, starty, width, height;
	height = LINES - 2;
	width  = COLS - 2;
	starty = (LINES - height) / 4;  /* Calculating for a center placement */
	startx = (COLS - width) / 2;    /* of */
	refresh();
	win = create_newwin(height, width, starty, startx);
	if (win == NULL) {
		zlog_info(c, "Window Was null...\n");
		endwin();
		printf("Window was null??\n");
		return -1;
	}

	zlog_info(c, "Size of window is %d by %d and the window handle is  %p \n", COLS, LINES,  win);
	RefreshData();
	refresh();
	wrefresh(win);

	int ch;
	while ((ch = getch()) != KEY_F(4)) {
		zlog_info(c, "Trapped Keypress  %d", ch );
		MEVENT event;
		switch (ch) {
		case KEY_RESIZE:
			zlog_info(c, "Resize()");
			refresh();
			wrefresh(win);
			break;
		case KEY_F(3):
			zlog_info(c, "F3=> Refresh");
			RefreshData();
			break;
		case KEY_LEFT:
			zlog_info(c, "Left key");
			break;
		case KEY_RIGHT:
			zlog_info(c, "Right key");
			break;
		case KEY_UP:
			zlog_info(c, "Up Key");
			break;
		case KEY_DOWN:
			zlog_info(c, "Down Key");
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
