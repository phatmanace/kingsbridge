
#include <stdio.h>
#include <curl/curl.h>
#include <string.h>
#include <ctype.h>
#include <stdlib.h>
#include <libconfig.h>
#include "hnarticle.h"
#include "hnfetcher.h"
#include <panel.h>
#include "zlog.h"
#include <regex.h>
#include <pthread.h>
#include <unistd.h>

#define BORDER 3
#define TRANSLATE(x)  x - BORDER

struct article **master_list;
WINDOW *create_newwin(int height, int width, int starty, int startx);
void destroy_win(WINDOW *local_win);
void refreshWin();
char *myStrCat(char *s, char *a);
char *repeatStr(char *str, size_t count);
int scrollAndAdjustViewPort(int direction, int length);
void openLink(struct article **list);

bool inputmode = FALSE;
bool shouldexit = FALSE;
bool amSearching = FALSE;

enum FOOTER_MODE {
	NORMAL,
	SEARCH

} footer_mode;

enum SCROLL_DIRECTION {
	SCROLL_DIRECTION_DOWN,
	SCROLL_DIRECTION_UP

};

char *searchText;
char *statusText;
char *currentLink  = NULL;

typedef struct {
	int start;
} listwindow;

typedef struct application_properties {
	int comment_display_active;
	char *active_story;
	WINDOW *main_window;
	int startx;
	int starty;
	int height;
	int width;
}application_properties;

zlog_category_t *c;
int selected = BORDER;
enum FOOTER_MODE fmode = NORMAL;
bool searchingSuspended = 1;
listwindow viewport;

application_properties* props;

/* mark the selected rows as being 'chosen'
 */

void markSelectedChosen(struct article **list)
{
	if (ArticleGet(*list, viewport.start + selected - BORDER) != NULL) {
		if (ArticleGet(*list, viewport.start + selected - BORDER)->flags & 32) {
			zlog_info(c, "Clearing chosen Flag..");
			ArticleGet(*list, viewport.start + selected - BORDER)->flags = ArticleGet(*list, viewport.start + selected - BORDER)->flags & ~(1 << 5);
		}else{
			zlog_info(c, "Setting chosen Flag..");
			ArticleGet(*list, viewport.start + selected - BORDER)->flags = ArticleGet(*list, viewport.start + selected - BORDER)->flags | (1 << 5);
		}
	}
}


/*
 * refresh loop called from a thread
 */

void *refreshLoop(void *data)
{
	struct FetchResult *result = malloc(sizeof(struct FetchResult));

	result->articles = NULL;
	zlog_info(c, "Initializing Thread %d", ArticleSize(result->articles));
	sleep(2);
	while (!shouldexit) {
		if (!inputmode) {
			amSearching = TRUE;
			refreshWin(BORDER, BORDER);
			zlog_info(c, "Fetching Articles:%d", TRANSLATE(6));
			sleep(2);
			//ClearList(list);
			FetchArticles(result);
			if (result->curl_result == RESULT_FAILED) {
				zlog_error(c, "Article fetch failed...");
				statusText = malloc(strlen("Article Fetch Failed") + 1);
				strcpy(statusText, "Article Fetch Failed");
			}else{
				zlog_info(c, "Fetched all articles ok... (%d total fetched)", ArticleSize(result->articles));
				//DumpArticles(result->articles);
			}
			amSearching = FALSE;
			master_list = &(result->articles);
			data = &(result->articles);

			refreshWin( BORDER, BORDER);
		}
		sleep(50);
	}
	return 0;

}

void init_window_props(void)
{
	props->comment_display_active = 0;
}
int main(void)
{
	int l_init = 0;

	props = malloc(sizeof(struct application_properties));
	init_window_props();
	statusText = malloc(strlen(" ") + 1);
	strcpy(statusText,         " ");
	l_init = zlog_init("/etc/zlog.conf");
	if (l_init) {
		printf("logging init failed");
		return -1;
	}
	c = zlog_get_category("hngui");

	zlog_info(c, "Program starting... selected is initially %d", selected);

	config_t cfg;

	config_init(&cfg);
	struct article *head = malloc(sizeof(struct article));
	int r = init_articles(head);
	struct article *tlist = head;
	master_list = &tlist;
	if (r != 0) {
		printf("Failed initilizting articles\n");
		exit(-1);
	}
	const char *mode;

	if (!config_read_file(&cfg, "/home/ace/hn_gui/curses/hn.cfg")) {
		fprintf(stderr, "%s:%d - %s\n", config_error_file(&cfg),
			config_error_line(&cfg), config_error_text(&cfg));
		config_destroy(&cfg);
		return(EXIT_FAILURE);
	}

	if (config_lookup_string(&cfg, "hn.mode", &mode)) {
		printf("Mode is %s\n", mode);
	} else {
		printf("mode not set\n");
	}
	config_destroy(&cfg);



	searchText = malloc(30);
	strcpy(searchText, "None");
	viewport.start = 0;
	if (!c) {
		printf("logging category init failed");
		return -1;
	}
	zlog_info(c, "Initializing Program");

	pthread_t fetcher_thread;

	if (pthread_create(&fetcher_thread, NULL, refreshLoop, &tlist)) {
		zlog_info(c, "Error creating thread");
		return -1;
	}



	int ch;
	initscr();
	start_color();
	init_pair(1, COLOR_YELLOW, COLOR_BLACK);
	init_pair(2, COLOR_BLACK, COLOR_WHITE);
	init_pair(3, COLOR_BLUE, COLOR_WHITE);
	init_pair(4, COLOR_RED, COLOR_BLACK);
	init_pair(5, COLOR_WHITE, COLOR_BLACK);
	init_pair(6, COLOR_BLACK, COLOR_YELLOW);
	cbreak();
	noecho();
	nonl();
	curs_set(0);
	keypad(stdscr, TRUE);

	props->height = LINES - 5;
	props->width = COLS - 5;
	props->starty = BORDER;
	props->startx = BORDER;
	props->main_window = create_newwin(props->height, props->width, props->startx, props->starty);
	refreshWin();

	while ((ch = getch()) != KEY_F(4)) {

		zlog_info(c, "Trapped Keypress  %d: %s (SZ=%d)", ch, searchText, ArticleSize(*master_list));
		if (fmode == SEARCH) {
			searchText[strlen(searchText)] = ch;
			searchText[strlen(searchText)] = '\0';
		}
		switch (ch) {
		case 'c':
			if (!inputmode) {
				zlog_info(c, "Cancelling Search mode...");
				memset(searchText, '\0', 30);
				ResetSearch(master_list);
				refreshWin( );
			}
			break;
		case 'M':
			zlog_info(c, "Activating comments");
			if (props->comment_display_active == 0) {
				props->comment_display_active = 1;
			}else{
				props->comment_display_active = 0;
			}

			refreshWin();
			break;
		case 's':
			if (!inputmode) {
				zlog_info(c, "Search mode...");
				memset(searchText, '\0', 30);
				curs_set(2);
				fmode = SEARCH;
				inputmode = TRUE;
				refreshWin();
			}
			break;
		case '/':
			if (!inputmode) {
				zlog_info(c, "Search mode...");
				memset(searchText, '\0', 30);
				curs_set(2);
				fmode = SEARCH;
				inputmode = TRUE;
				refreshWin();
			}
			break;
		case 'o':
			if (!inputmode) {
				zlog_info(c, "Opening link");
				openLink(master_list);
				refreshWin();
			}
			break;
		case 'R':
			if (!inputmode) {
				zlog_info(c, "Resetting");
				selected = BORDER + 1;
				viewport.start = 0;
				refreshWin();
			}
			break;
		case KEY_HOME:
			zlog_info(c, "Home Key Trapped");
			break;
		case ' ':
			zlog_info(c, "Trapped space");
			markSelectedChosen(master_list);
			if (scrollAndAdjustViewPort(SCROLL_DIRECTION_DOWN, ArticleSize(*master_list)) == 1) {

			}
			refreshWin();
			break;
		case KEY_LEFT:
			break;
		case KEY_RIGHT:
			break;
		case KEY_UP:
			zlog_info(c, "First Article ==> %s", ArticleGet(*master_list, 0)->link);
			if (scrollAndAdjustViewPort(SCROLL_DIRECTION_UP, ArticleSize(*master_list)) == 1) {
			}else{
				zlog_info(c, "scrolled_up, but didn't actually do anything\n");
			}
			zlog_info(c, "Selected is now %d", selected);
			refreshWin();
			break;
		case 13:
			zlog_info(c, "Cancelling input mode");
			int len = 0;
			for (len = strlen(searchText) - 1; len < 30; len++) {
				searchText[len] = '\0';
			}
			searchText[strlen(searchText)] = '\0';
			inputmode = FALSE;
			fmode = NORMAL;
			zlog_info(c, "Actually constructing search..[%s]", searchText);
			applySearch(master_list, searchText);
			SortList(master_list);
			zlog_info(c, "Finished Sorting list..");
			curs_set(0);
			refreshWin();
			break;
		case KEY_BACKSPACE:
			zlog_info(c, "Key Delete");
			if (fmode == SEARCH) {
				int len = 0;
				for (len = strlen(searchText) - 1; len < 30; len++) {
					searchText[len] = '\0';
				}
				searchText[strlen(searchText) - 1] = '\0';
				zlog_info(c, "Search Text is now %s, %zu", searchText, strlen(searchText));
			}
			break;
		case KEY_RESIZE:
			props->height = LINES - 5;
			props->width = COLS - 5;
			zlog_info(c, "Resize...");
			destroy_win(props->main_window);
			endwin();
			refresh();
			clear();
			refreshWin();
			break;
		case KEY_PPAGE:
			zlog_info(c, "Page up...");
			selected = BORDER + 1;
			refreshWin();
			break;
		case KEY_NPAGE:
			zlog_info(c, "Page down...");
			selected = LINES - (2 * BORDER) + 2;
			refreshWin();
			break;
		case KEY_DOWN:
			zlog_info(c, "key_down() First Article ==> %s", ArticleGet(*master_list, 0)->link);
			if (scrollAndAdjustViewPort(SCROLL_DIRECTION_DOWN, ArticleSize(*master_list)) == 1) {
			}
			refreshWin();
			break;
		}
	}

	destroy_win(props->main_window);
	endwin();               /* End curses mode		  */
	zlog_fini();
	return 0;

}



int scrollAndAdjustViewPort(int direction, int length)
{
	zlog_info(c, "(%d) SELECTED_INDEX: %d, from bottom: %d, from top: %d, inc: %d ", direction, selected, LINES - BORDER -  selected, BORDER - selected, selected + viewport.start);
	//down

	if (direction == SCROLL_DIRECTION_DOWN) {
		if (selected + viewport.start == length + BORDER + 4) {
			zlog_info(c, "Hit the bottom");
			return 1;
		}
		if (LINES - BORDER - selected == 1) {
			viewport.start++;
			return 1;
		} else {
			selected++;
			return 0;
		}
	} else {
		if (selected + viewport.start ==  BORDER) {
			zlog_info(c, "Hit the top");
			return 1;
		}
		if (BORDER - selected == 0 || BORDER - selected == -1) {
			viewport.start--;
			return 1;
		} else {
			selected--;
			return 0;
		}

	}

}


void openLink(struct article **list)
{
	char call[256];

	strcpy(call, "xdg-open "); // web browser command
	strcat(call, currentLink);
	strcat(call, "> /dev/null");
	system(call);
}

void drawHeader()
{
	if (props->comment_display_active == 1) {
		mvwprintw(props->main_window, 2, 1, "[C]Commands: [(O)pen] [(R)eset|(r)eload]");
	}else{
		mvwprintw(props->main_window, 2, 1, "  Commands: [(O)pen] [(R)eset|(r)eload]");
	}
	mvwprintw(props->main_window, 2, 80, "                                                  ");
	mvwprintw(props->main_window, 2, 80, "Search Text:%s", searchText);
	mvwprintw(props->main_window, 2, 100, "sel:%d cho:%d", selected, selected - BORDER);
}

void drawFooter(struct article *list)
{


	char *space = repeatStr(" ", COLS);
	int FOOTER_ROW = LINES - 7;

	mvwprintw(props->main_window, FOOTER_ROW, props->startx + 1, space);
	free(space);

	if (currentLink != NULL) {
		free(currentLink);
	}
	currentLink = malloc(256);

	if (ArticleSize(list) > 0 && ArticleGet(list, viewport.start + selected - BORDER) != NULL) {
		strcpy(currentLink,  ArticleGet(list, viewport.start + selected - BORDER)->link);
	}


	if (amSearching) {
		mvwprintw(props->main_window, FOOTER_ROW, props->startx + 1, "Searching for articles...");
	} else {


		switch (fmode) {
		case NORMAL:
			zlog_info(c, "All Refreshed selected is %d", viewport.start +  selected - BORDER);
			mvwprintw(props->main_window, FOOTER_ROW, props->startx + 1, "Selected [%5d/%-5d] %30s [%d] %s",
				  selected + viewport.start - BORDER,
				  ArticleSize(list),
				  ArticleGet(list, viewport.start + selected - BORDER) == NULL ? "" : ArticleGet(list, viewport.start + selected - BORDER)->link,
				  ArticleGet(list, viewport.start + selected - BORDER) == NULL ? 0  : ArticleGet(list, viewport.start + selected - BORDER)->flags,
				  statusText
				  );
			if (ArticleGet(list, viewport.start + selected - BORDER) != NULL) {
				ArticleGet(list, viewport.start + selected - BORDER)->flags = ArticleGet(list, viewport.start + selected - BORDER)->flags | (1 << 3);
			}
			break;
		case SEARCH:
			echo();
			mvwprintw(props->main_window, FOOTER_ROW, props->startx + 1, "Search for:");
			break;

		}
	}
}

void refreshWin()
{

	int max = LINES - (2 * BORDER) - 5;
	char* saved = malloc(4);
	struct article **list = master_list;

	zlog_info(c, "__________Main Window Refresh loop________\nRefreshing main window (S:%d) (M:%d) (TOTAL_LIST_SIZE:%d)", selected, max, ArticleSize(*list));
	//xDumpArticles("", *list);
	if (selected <=  BORDER) {
		selected = BORDER + 1;
		return;
	}
	if (selected > ArticleSize(*list) + BORDER) {
		selected = BORDER + 1;
	}

	if (max > ArticleSize(*list)) {
		max = ArticleSize(*list);
	}

	drawHeader();
	box(props->main_window, ACS_VLINE, ACS_HLINE);          /* 0, 0 gives default characters */
	//mvwhline(props->main_window, 1,2,ACS_HLINE, 40);
	wrefresh(props->main_window);


	int s_row = props->starty + 1;
	struct article *firsta = *list;
	int index = 1;
	zlog_info(c, "Viewport Start...%d", viewport.start);
	if (viewport.start > 0) {
		index++;
		int slide = 0;
		for (slide = 0; slide < viewport.start; slide++) {
			zlog_info(c, "Sliding forward...");
			firsta = firsta->next;
		}
	}
	int pair = 1;
	int s_index = 0;
	zlog_info(c, "First row being added is %s, %zu", firsta->text, sizeof(int));

	/*
	    Show articles...
	 */
	while (firsta != NULL && s_index < max) {
		char* chosen = malloc(4);
		char* b_flags = malloc(9);
		s_index++;
		if (s_row == selected) {
			pair = 2;
		} else {
			if (firsta->flags & 1) {
				pair = 4;
			} else {
				pair = 1;
			}
		}
		if (firsta->flags & 32) {
			strcpy(chosen, "[C]");
			pair = 6;
		}else{
			strcpy(chosen, "   ");
		}



		if (firsta->flags & 16) {
			strcpy(saved, "(S)");
		}else{
			strcpy(saved, "   ");
		}
		if (s_row == selected) {
			pair = 2;
		}
		wattron(props->main_window, COLOR_PAIR(3));
		mvwprintw(props->main_window, s_row, props->startx + 3, "%3d", s_index + viewport.start);
		wattroff(props->main_window, COLOR_PAIR(3));
		wattron(props->main_window, COLOR_PAIR(pair));
		mvwprintw(props->main_window, s_row, props->startx + 7, "%s",  "                                                                                    ");
		mvwprintw(props->main_window, s_row, props->startx + 7, "%s %s  %s %100s (F:%d,s_i:%d,i:%d)",  saved, int2bin(firsta->flags, b_flags, 9), chosen, firsta->text, firsta->flags, s_index, index);
		firsta = firsta->next;
		s_row++;
		index++;
		wattroff(props->main_window, COLOR_PAIR(pair));
		free(chosen);
	}
	drawFooter(*list);
	refresh();
	if (props->comment_display_active == 1) {
		zlog_info(c, "Dispaying comment window");
		WINDOW *local_win;
		local_win = newwin(100, 100, 100, 100);
		box(local_win, 0, 0);
		mvwprintw(local_win, 1, 1, "Aceman");
		wrefresh(local_win);
	}
	free(saved);
	zlog_info(c, "__________END_REFRESH_LOOP()___________\n");
	wrefresh(props->main_window);

}

char *myStrCat(char *s, char *a)
{
	while (*s != '\0') s++;
	while (*a != '\0') *s++ = *a++;
	*s = '\0';
	return s;
}

char *repeatStr(char *str, size_t count)
{
	if (count == 0) return NULL;
	char *ret = malloc(strlen(str) * count + count);
	if (ret == NULL) return NULL;
	*ret = '\0';
	char *tmp = myStrCat(ret, str);
	while (--count > 0) {
		tmp = myStrCat(tmp, " ");
		tmp = myStrCat(tmp, str);
	}
	return ret;
}

WINDOW *create_newwin(int height, int width, int starty, int startx)
{
	WINDOW *local_win;

	local_win = newwin(height, width, starty, startx);
	box(local_win, ACS_VLINE, ACS_HLINE);   /* 0, 0 gives default characters
	                                         * lines			*/
	wrefresh(local_win);                    /* Show that box                */

	return local_win;
}

void destroy_win(WINDOW *local_win)
{
	/* box(local_win, ' ', ' '); : This won't produce the desired
	 * result of erasing the window. It will leave it's four corners
	 * and so an ugly remnant of window.
	 */
	wborder(local_win, ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ');
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
