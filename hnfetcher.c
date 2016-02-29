
#include <stdio.h>
#include <curl/curl.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <regex.h>
#include <libconfig.h>
#include "hnarticle.h"
#include "hnfetcher.h"
#include "yajl/yajl_tree.h"
#include "zlog.h"


struct string {
	char *ptr;
	size_t len;
};





zlog_category_t *c;
size_t appendHTMLChunk(void *ptr, size_t size, size_t memb, struct string *s)
{
	size_t newlen = s->len + (size * memb);

	s->ptr = realloc(s->ptr, newlen + 1);
	if (s->ptr == NULL) {
		fprintf(stderr, "realloc() failed\n");
		exit(EXIT_FAILURE);
	}
	memcpy(s->ptr + s->len, ptr, size * memb);
	s->ptr[newlen] = '\0';
	s->len = newlen;
	return size * memb;

}

void init_string(struct string *s)
{
	s->len = 0;
	s->ptr = malloc(s->len + 1);
	if (s->ptr == NULL) {
		fprintf(stderr, "malloc() failed\n");
		exit(EXIT_FAILURE);
	}
	s->ptr[0] = '\0';
}

int init_articles(struct article *a)
{

	c = zlog_get_category("hngui");
	if (!c) {
		printf("zlog_initialization failure\n");
		zlog_fini();
		return -2;
	}
	a->next          = NULL;
	a->previous      = NULL;
	a->text          = '\0';
	a->storyhash     = NULL;
	a->link          = NULL;
	a->init          = -1;
	a->flags         = 0;
	zlog_info(c, "Initialized List...");
	return 0;

}

struct article* ArticleGet(struct article *list, int x)
{
	int i = 0;
	struct article *tmp = list;

	while (i != x && tmp != NULL) {
		tmp = tmp->next;
		i++;
	}
	return tmp;

}

int ArticleSearchByHash(struct article *list, const char* hash)
{
	int i = 0;
	//zlog_info(c, "Searching By Hash..{%s}", hash);
	struct article *tmp = list;

	if (list == NULL || list->storyhash == NULL || hash == NULL || strlen(hash) == 0) {
		return 0;
	}


	while (tmp != NULL) {
		//zlog_info(c, "%d Loop....Searching By Hash..%p", i, tmp);
		if (tmp->init != 0 && strcmp(tmp->storyhash, hash) == 0) {
			return 1;
		}
		tmp = tmp->next;
		i++;
	}
	return 0;

}

void ClearList(struct article *list)
{

	// find tail.
	struct article *tmp = list;
	struct article *elim = NULL;

	while (tmp->next != NULL) {
		tmp = tmp->next;
	}

	//tmp is now at the end of the list;

	while (tmp->previous != NULL) {
		elim = tmp;
		free(elim->link);
		free(elim->text);
		free(elim->storyhash);
		free(elim);
		tmp = tmp->previous;
		tmp->next = NULL;
	}


}
void add_article_by_hash(struct article **list, char *text, char *link, char *hash, char* saved)
{


	zlog_info(c, "Adding new Article By hash %s", hash);
	//xDumpArticles("<inner>", *list);
	int exists = ArticleSearchByHash(*list, hash);

	//zlog_info(c, "Adding article %s (%d) (%d)", link, exists, ArticleSize(*list) );
	if (exists == 1) {
		zlog_info(c, "Article Already exists, skipping...");
		return;
	}
	//zlog_info(c, "Adding a new article");

	if ((*list)->init <= 0) {
		zlog_info(c, "Add Article..init mode.... (%p)", list);
		(*list)->init++;
		(*list)->flags = 0;
		(*list)->link = malloc(strlen(link) + 1);
		strcpy((*list)->link, link);

		(*list)->text = malloc(strlen(text) + 1);
		strcpy((*list)->text, text);

		(*list)->storyhash = malloc(strlen(hash) + 1);
		strcpy((*list)->storyhash, hash);
		(*list)->other = malloc(1);
		strcpy((*list)->other, "-");


	} else {

		struct article *newnode = malloc(sizeof( struct article));
		zlog_info(c, "add_article_non_init() recentlly allocated node is (%s) %p   (nn=%p) ", text, *list, newnode);
		if (newnode == NULL) {
			zlog_info(c, "Error mallocing new node");
			return;
		}
		newnode->other = malloc(1);
		strcpy(newnode->other, "-");
		newnode->flags = 0;
		if (saved[0] == 'y') {
			zlog_info(c, "Located Saved Article! %s", link);
			newnode->flags = newnode->flags | (1 << 4);
		}
		newnode->next = NULL;
		newnode->previous = NULL;
		newnode->init = 1;
		if (newnode == NULL) {
			printf("Malloc() failed");
		}

		newnode->link = malloc(strlen(link) + 1);
		strcpy(newnode->link, link);

		newnode->text = malloc(strlen(text) + 1);
		strcpy(newnode->text, text);

		newnode->storyhash = malloc(strlen(hash) + 1);
		strcpy(newnode->storyhash, hash);
		newnode->next = NULL;
		// join onto the end of the other list.

		struct article *tmpnode = *list;
		while (tmpnode->next != NULL) {
			tmpnode = tmpnode->next;
		}

		newnode->previous = tmpnode;
		tmpnode->next = newnode;
		zlog_info(c, "(newhead=%p,list=%p, title=%s)", newnode, *list, (*list)->link);
	}

}

void add_article(struct article *list, char *text, char *link, char *hash)
{


	struct article *tempnode = malloc(sizeof( struct article));

	list->init++;
	tempnode->init = 0;
	tempnode->flags = 0;
	if (tempnode == NULL) {
		printf("Malloc() failed");
	}

	tempnode->link = malloc(strlen(link) + 1);
	strcpy(tempnode->link, link);

	tempnode->other = malloc(1);
	strcpy(tempnode->other, "-");

	tempnode->text = malloc(strlen(text) + 1);
	strcpy(tempnode->text, text);

	tempnode->storyhash = malloc(strlen(hash) + 1);
	strcpy(tempnode->storyhash, hash);
	tempnode->next = NULL;

	struct article *newhead = malloc(sizeof(struct article));
	if (list->next != NULL) {
		newhead = list->next;
		while (newhead->next != NULL) {
			newhead = newhead->next;
		}
		//printf("Done Finding head of list\n");
	} else {
		//printf("Already at the head of the list\n");
		newhead = list;
	}

	tempnode->previous = newhead;
	newhead->next = tempnode;

}

int ArticleSize(struct article *list)
{
	int x = 0;
	struct article *newhead = list;

	if (list == NULL || list->init == 0 || list->init == -1) {
		return 0;
	}
	while (newhead->next != NULL && x < 100000 ) {
		if (newhead->text != NULL && strlen(newhead->text) > 0) {
			x++;
		}
		newhead = newhead->next;
		if (newhead == NULL) {
			break;
		}
	}
	if (x >= 100000) {
		zlog_info(c, "Alarm - size exceeds maximum");
	}
	return ++x;


}

void applySearch(struct article **list, char* searchText)
{
	zlog_info(c, "Starting search for matches: %s (len: %zu)", searchText, strlen(searchText));
	regex_t regex;
	int reti = regcomp(&regex, searchText, REG_ICASE);
	if (reti) {
		zlog_info(c, "Failed to compile regex");
		return;
	}
	int matchcount = 0;
	int scanned = 0;
	struct article *firsta = *list;
	while (firsta->next != NULL) {
		//zlog_info(c, "Trying to match %s against %s, %zu:", firsta->text, searchText, strlen(searchText));
		firsta->flags = firsta->flags & ~(1);
		if (firsta->text != '\0' && !regexec(&regex, firsta->text, 0, NULL, 0)) {
			zlog_info(c, "Match: %s", firsta->text);
			matchcount++;
			firsta->flags = firsta->flags | 1;
		}
		firsta = firsta->next;
		scanned++;
	}


	zlog_info(c, "Done Starting search: %d matches after %d scans", matchcount, scanned);
	regfree(&regex);


}

void ResetSearch(struct article **list)
{
	zlog_info(c, "Resetting search ");
	struct article *firsta = *list;
	while (firsta->next != NULL) {
		firsta->flags = firsta->flags & 0;
		firsta = firsta->next;
	}
	zlog_info(c, "Done Resetting search: ");


}
void SortList(struct article **list)
{
	struct article *sortedList = NULL;
	struct article *unsortedList = NULL;

	zlog_info(c, "Sorting list after search");
	int sweeping = 1;
	int iterations = 1;
	//struct article *n = NULL;
	while (sweeping > 0 && iterations < 100) {
		zlog_info(c, "Iterate..");
		sweeping = 0;
		struct article *t = *list;
		struct article *i = *list;
		struct article *a = *list;
		while (i != NULL) {
			t = i;
			//zlog_info(c, "[[%d]]------Examining node (t=%p, t_f=%d, t_n=%p, t_p=%p) ",iterations, t, t->flags, t->next, t->previous);
			if ((t->flags & 1 ) == 1) {
				zlog_info(c, "[[%d]]------**node_match. nodes  (tx=%s, t=%p, t_f=%d, t_n=%p, t_p=%p) ", iterations, t->text, t, t->flags, t->next, t->previous);
				a = t->next;
				if (t->previous != NULL) {
					t->previous->next = t->next;
				}
				//t->text[0] = 'Y'; t->text[1] = '/';
				t->next = NULL;
				if (sortedList == NULL) {
					zlog_info(c, "----------------First match that we need to account for %s\n", t->text);
					sortedList = t;
					sortedList->previous = NULL;
				}else{
					sortedList->next = t;
					t->previous = sortedList;
					sortedList = t;
				}
				zlog_info(c, "[[%d]] ------------[sort_match_a]..%s (h=%p)", iterations, t->text,  sortedList);
			} else {
				zlog_info(c, "[[%d]] Node %p(%s) didn't need to be sorted - adding to unsorted list", iterations, t, t->text);
				a = t->next;
				//t->text[0] = '+'; t->text[1] = '/';
				t->next = NULL;
				if (unsortedList == NULL) {
					zlog_info(c, "First append to unsorted list %s\n", t->text);
					unsortedList = t;
					unsortedList->previous = NULL;
				}else{
					unsortedList->next = t;
					t->previous = unsortedList;
					unsortedList = t;
				}
			}
			i = a;
			zlog_info(c, "(Loop_end) t=%p, i=%p", t, i);
		}
		zlog_info(c, "Iterating through list again.(%d).", iterations);
		iterations++;
	}
	zlog_info(c, "Sorting is finished...");
	struct article *t = unsortedList;

	while (t->previous != NULL) {
		t = t->previous;
	}
	if (sortedList != NULL) {
		sortedList->next = t;
		t->previous = sortedList;
		while (sortedList->previous != NULL) {
			sortedList = sortedList->previous;
		}
		*list = sortedList;
		zlog_info(c, "Setting prevous to (t_p=%p)", t->previous);
	}else{
		*list = t;
	}
	zlog_info(c, "-------------------sort complete-------------------------(h=%p) (h_n=%p) (l=%p)--(t_prev=%p)\n", t, t->next, *list, t->previous);
	zlog_info(c, "_________________________________________");
	xDumpArticles("outer_sort", *list);

}
void xDumpArticles(char* prefix, struct article *list)
{
	if (list == NULL) {
		zlog_info(c, "List null..");
		printf("list null\n");
		return;
	}
	//printf("[%s] Dumping Articles... %-15p..........(Size=%d)......\n", prefix, list, ArticleSize(list));
	//printf("-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.\n");
	zlog_info(c, "Sprint..");
	zlog_info( c, "[%s] Dumping Articles... %-15p..........(Size=%d)......", prefix, list, ArticleSize(list));
	zlog_info( c, "-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.");
	struct article *tmp = list;
	int i = 0;
	while (tmp != NULL) {
		i++;
		char *b_flags = malloc(9);
		//printf("[%s %-4d.] %-25s %-15s (this=%p) (n=%p) (%d) [p=%p]\n",prefix, i, (tmp->text), tmp->link, tmp, tmp->next, tmp->flags, tmp->previous);
		zlog_info(c, "(%20p) (%20p) %s [%s %d.] %-15s %-20s ", tmp, tmp->next, int2bin(tmp->flags, b_flags, 9), prefix, i, (tmp->text), tmp->link );
		tmp = tmp->next;
		usleep(10 * 1000);
	}
	zlog_info(c, "------- Dumping Complete-----------\n");
}
void DumpArticles( struct article *list)
{
	xDumpArticles("", list);
}
/*
 * Show an int as a binary string
 */

char *int2bin(int a, char *buffer, int buf_size)
{
	int i;

	for (i = 0; i < 9; i++) {
		if (a & ( 1 << i)) {
			buffer[i] = '1';
		}else{
			buffer[i] = '-';
		}
	}
	buffer[8] = '\0';
	//zlog_info(c, "Returning as bit conversion |%s|", buffer);
	return buffer;
}

void FetchArticles(struct FetchResult *result)
{

	if (result->articles == NULL) {
		struct article *head = malloc(sizeof( struct article));
		head->init = 0;
		head->next = NULL;
		result->articles = head;
	}

	CURL *curl;
	CURLcode res;
	char errbuf[1024];
	zlog_info(c, "Hitting Server for articles");

	curl = curl_easy_init();
	if (curl) {
		struct string s;
		init_string(&s);
		curl_easy_setopt(curl, CURLOPT_URL, "http://localhost:3000/api/posts");
		/* example.com is redirected, so we tell libcurl to follow redirection */
		curl_easy_setopt(curl, CURLOPT_FOLLOWLOCATION, 1L);
		curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, appendHTMLChunk);
		curl_easy_setopt(curl, CURLOPT_WRITEDATA, &s);

		/* Perform the request, res will get the return code */
		res = curl_easy_perform(curl);
		/* Check for errors */
		if (res != CURLE_OK) {
			zlog_error(c, "Failed to contact server for articles... %s", curl_easy_strerror(res));
			result->curl_result = RESULT_FAILED;
			return;
		}

		yajl_val node = yajl_tree_parse((const char*)s.ptr, errbuf, sizeof(errbuf));
		result->curl_result = RESULT_OK;
		if (node == NULL) {
			zlog_info(c, "Parser Errored");
			result->curl_result = RESULT_FAILED;
		}

		if (node && YAJL_IS_ARRAY(node)) {
			size_t len = node->u.array.len;
			int i;
			for (i = 0; i < len; i++) {
				yajl_val _article = node->u.array.values[i];
				//zlog_info(c, "Found article %s %s %s %s", _article->u.object.keys[0], _article->u.object.keys[1], _article->u.object.keys[2],_article->u.object.keys[3]);
				yajl_val title = _article->u.object.values[0];
				yajl_val guid = _article->u.object.values[1];
				yajl_val link = _article->u.object.values[2];
				yajl_val saved = _article->u.object.values[3];
				zlog_info(c, " ART..(%zu) %d [%zu] t:%s l:%s g:%s sa:%s", node->u.array.len, i, _article->u.object.len, YAJL_GET_STRING(title), YAJL_GET_STRING(link), YAJL_GET_STRING(guid), YAJL_GET_STRING(saved));
				add_article_by_hash(&(result->articles), YAJL_GET_STRING(title), YAJL_GET_STRING(link), YAJL_GET_STRING(guid), YAJL_GET_STRING(saved));
				zlog_info(c, "-----------done Adding articles------------------\n");
			}

			DumpArticles(result->articles);

		}else{
			result->curl_result = RESULT_FAILED;
		}
		zlog_info(c, "Article Size after curl call is %d", ArticleSize(result->articles));
		curl_easy_cleanup(curl);
	}

}
