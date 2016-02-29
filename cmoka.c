#include <stdio.h>
#include <stdlib.h>
#include "hnarticle.h"
#include "hnfetcher.h"
#include "zlog.h"
#include <regex.h>
#include <string.h>

zlog_category_t *c;

int main(void)
{
	printf("Checking..\n");
	int l_init = 0;
	l_init = zlog_init("/etc/zlog.conf");
	c = zlog_get_category("hngui");
	if (!c) {
		printf("zlog_initialization failure\n");
		zlog_fini();
		return -2;
	}
	if (l_init) {
		printf("logging init failed");
		return -1;
	}
	printf("logging..\n");
	struct FetchResult *result = malloc(sizeof(struct FetchResult));
	result->articles = NULL;
	FetchArticles(result);
	printf("Dumping....\n");
	DumpArticles(result->articles);
	ArticleGet(result->articles, 4)->flags |= (1 << 5);
	zlog_info(c, "----Set Flag on 4th element...-----------------");
	DumpArticles(result->articles);
	zlog_info(c, "----fetching articles again....----------------");
	FetchArticles(result);
	DumpArticles(result->articles);
	return 1;
}
void foo()
{
/*
    //FetchArticles(&list);
    zlog_info(c, "Current head node is %p", list);
    printf("here..\n");
    zlog_info(c, " About to add in first article");
    add_article_by_hash(&list, "text", "link1", "hash1");
    printf("Added 1st...\n");
    zlog_info(c, "Added one.. now adding another..%p %s\n", list, list->link);
    add_article_by_hash(&list, "text2", "link2", "hash2");
    zlog_info(c, "Added two.. now adding another..%p %s\n", list, list->link);
    add_article_by_hash(&list, "text_3" , "link3", "hash3");
    add_article_by_hash(&list, "text_42", "link42", "hash42");
    add_article_by_hash(&list, "text_x44", "linka44", "hash441");
    add_article_by_hash(&list, "text_y44", "linkb44", "hash442");
    add_article_by_hash(&list, "text_a42", "linkc42", "hash443");
    add_article_by_hash(&list, "text_b44", "linkd44", "hash444");
    add_article_by_hash(&list, "text_d42", "linke42", "hash445");
    add_article_by_hash(&list, "text_e44", "linkg44", "hash446");
    add_article_by_hash(&list, "text_f31", "linkh31", "hash317");
    zlog_info(c, "Added three.. finally printing list..%p %s\n", list, list->link);

    int originalSize = ArticleSize(list);

    DumpArticles(list);
    printf("--------------Searching now----------------\n");
    printf("\n\n\n");
    applySearch(&list, "2");
    DumpArticles(list);
    printf("--------------Sorting now----------------\n");
    zlog_info(c, "\n\n----\n\n---\n\n---");
    SortList(&list);
    zlog_info(c, "\n\n----\n\n---\n\n---");
    printf("--------------Sorting complete now----------------\n");
    int newSize = ArticleSize(list);
    xDumpArticles("Final", list);
    printf("Added two.. now adding another..\n");
    printf("Original size ..%d\n", originalSize);
    printf("New Size      ..%d\n", newSize);
    if(newSize != originalSize){
        printf("***********************error... something bad happened******************\n");
    }

 */
}
