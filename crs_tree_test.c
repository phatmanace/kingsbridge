#include <ncurses.h>

WINDOW *create_newwin(int height, int width, int starty, int startx);

int main(void)
{
	initscr();
	start_color();
	curs_set(0);
	init_pair(1,COLOR_YELLOW, COLOR_BLACK);

	int startx, starty, width, height;
	height = 30;
	width  = 100;
	starty = (LINES - height) / 4;	/* Calculating for a center placement */
	startx = (COLS - width) / 2;	/* of */
	refresh();
	WINDOW *win = create_newwin(height, width, starty, startx);
	wbkgd(win, COLOR_PAIR(1));
	refresh();
	int txcol = 7;
	wmove(win, 3, 1);
	wprintw(win, "Hello");
	wmove(win, 5, 1);
	whline(win, ACS_HLINE, 99); 
	mvwaddch(win, 5,0,  ACS_LTEE);
	mvwaddch(win, 5,99,  ACS_RTEE);
	mvwaddch(win, 6,2, ACS_URCORNER );
	wmove(win, 6, txcol);
	wprintw(win, "Wibble");
	mvwaddch(win, 7,2, ACS_LTEE );
	// line..
	wmove(win, 7, 3);
	whline(win, ACS_HLINE, 1); 
	
	wmove(win, 7, txcol); wprintw(win, "Wabcdfe");
	mvwaddch(win, 8,2, ACS_LLCORNER );
	mvwaddch(win, 8,3, ACS_URCORNER );
	mvwaddch(win, 9,3, ACS_LLCORNER );
	wmove(win, 8, txcol + 1); wprintw(win, "WLast Node");
	wmove(win, 9, txcol + 1); wprintw(win, "Last Node");
	refresh();
	wrefresh(win);

	getch();
	endwin();
	return 0;
}

WINDOW *create_newwin(int height, int width, int starty, int startx)
{	WINDOW *local_win;

	local_win = newwin(height, width, starty, startx);
	box(local_win, ACS_VLINE , ACS_HLINE);		/* 0, 0 gives default characters 
					 * for the vertical and horizontal
 * 					 * lines			*/
	wrefresh(local_win);		/* Show that box 		*/

	return local_win;
}
