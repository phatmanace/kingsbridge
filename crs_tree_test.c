#include <ncurses.h>
#include "comment_tree.h"
#include "t_sample.h"
#include "zlog.h"

zlog_category_t *c;
WINDOW *create_newwin(int height, int width, int starty, int startx);


void RenderRow(WINDOW* win, comment_item_tree* node)
{
    
            wmove(win, row++, txcol); wprintw(win, node->text);
}

void RenderTreeIntoWindow(WINDOW* win, comment_item_tree* tree)
{
	zlog_info(c, "Rendering window %dX%d", win->_maxy, win->_maxx);

        comment_item_tree* tmp = tree;
        int row = 4;
        int txcol = 7;
        while(tmp != NULL){
            RenderRow(win, tmp); 
            tmp = tmp->next;

        }


}

void SampleInfo()
{
	//int txcol = 7;
	//wmove(win, titlebarlocation, 1);
	//int titlebarlocation = 3;
	//whline(win, ACS_HLINE, 99);
	//mvwaddch(win, titlebarlocation, 0,  ACS_LTEE);
	//mvwaddch(win, titlebarlocation, 99,  ACS_RTEE);
	//mvwaddch(win, 6, 2, ACS_URCORNER );
	//wmove(win, 6, txcol);
	//wprintw(win, "Wibble");
	//mvwaddch(win, 7, 2, ACS_LTEE );
	//// line..
	//wmove(win, 7, 3); whline(win, ACS_HLINE, 1);

	//wmove(win, 7, txcol); wprintw(win, "Wabcdfe");
	//mvwaddch(win, 8, 2, ACS_LLCORNER );
	//mvwaddch(win, 8, 3, ACS_TTEE );
	//wmove(win, 8, 4); whline(win, ACS_HLINE, 2);
	//mvwaddch(win, 9, 3, ACS_LLCORNER );

	//wmove(win, 8, 4); whline(win, ACS_HLINE, 3);
	//wmove(win, 8, txcol + 1); wprintw(win, "WLast Node");
	//mvwvline(win, 8, 6, ACS_VLINE, 2 );
	////mvwaddch(win, 9,6, ACS_LLCORNER );
	//wmove(win, 9, txcol + 1); wprintw(win, "more last n");

	//mvwaddch(win, 9, 3, ACS_VLINE );
	//mvwaddch(win, 10, 3, ACS_LLCORNER );
	//wmove(win, 10, 4); whline(win, ACS_HLINE, 3);
	//wmove(win, 10, txcol + 1); wprintw(win, "Last Node");
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
	comment_item_tree* tree = make_sample_tree();
	printf("Tree Size: %d\n", TotalSize(tree));
	initscr();
	start_color();
	curs_set(0);
	init_pair(1, COLOR_YELLOW, COLOR_BLACK);
	init_pair(2, COLOR_RED, COLOR_BLACK);

	int startx, starty, width, height;
	height = 30;
	width  = 100;
	starty = (LINES - height) / 4;  /* Calculating for a center placement */
	startx = (COLS - width) / 2;    /* of */
	refresh();
	WINDOW *win = create_newwin(height, width, starty, startx);
	wbkgd(win, COLOR_PAIR(1));
	refresh();
	wmove(win, 2, 1);
	wattron(win, A_BOLD | A_BLINK);
	wattron(win, COLOR_PAIR(2));
	wprintw(win, "News Comments for ... %d ", TotalSize(tree) );
	wattroff(win, COLOR_PAIR(2));

	wattroff(win, A_BOLD | A_BLINK);


	RenderTreeIntoWindow(win, tree);
	refresh();
	wrefresh(win);

	getch();
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
