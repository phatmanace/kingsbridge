
#include <stdio.h>
#include <sys/time.h>
#include <stdlib.h>
#include <string.h>
#include <pthread.h>
#include <unistd.h>

#include "queue.h"
#include "comment_tree.h"
#include "string_utils.h"
#include "curl/curl.h"
#include "cjson/cJSON.h"
#include "yajl/yajl_tree.h"
#include "jansson.h"
#include <stdarg.h>
#include "downloader.h"

#define RUNS 10000
#define NUMT 1


pthread_mutex_t lock;




void blip(char* format, ...){

	#ifdef _S_CURSES_DEBUG 
	va_list args;
	va_start(args, format);
	printf("t_curses_multi_test()::");
	vprintf(format, args);
	va_end(args);
	#endif
}




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

void *downloadSingleURL(void *x)
{
	struct thread_args* args = (struct thread_args*)x;
	pthread_mutex_t lock = args->lock;
	if(args->callback == NULL){
		printf("Abort - callback function was null\n");
		return (void*)0;
	}


	unsigned int self = (unsigned int)pthread_self();

	args->callback("[%u] In Thread loop\n", self);
	CURL *curl;
	curl = curl_easy_init();
	if (!curl) {
		args->callback("Exiting... Curl didn't seem to init correctly\n");
		return (void*)0;
	}
	args->callback("[After init] In Thread loop\n");
	if (args->queue == NULL) {
		args->callback("Queue was null...");
	}
	;
	args->callback("[%u] Starting up thread while()..\n", self);
	int tries = 3;
	while (tries > 0) {
		while (QSize(args->queue) > 0) {
			if(tries == 1){
				tries++;
			}
			struct string response_string;
			init_string(&response_string);

			args->callback("[T:%u] before pop queue size is now %d\n", self,  QSize(args->queue));
			int hnArticle = QpopItem(args->queue, &lock);
			if (hnArticle == -1) {
				args->callback("[%u] Blank Article... assume Queue was empty\n", self);
				continue;
			}
			char* url = malloc(sizeof(char) * 100);

			sprintf(url, "https://hacker-news.firebaseio.com/v0/item/%d.json", hnArticle);
			args->callback("[T:%u] %s z:%d\n", self,  url, QSize(args->queue));
			printf("[T:%u] %s z:%d\n", self,  url, QSize(args->queue));
			curl_easy_setopt(curl, CURLOPT_URL, url);
			curl_easy_setopt(curl, CURLOPT_TIMEOUT, 5L);
			curl_easy_setopt(curl, CURLOPT_FOLLOWLOCATION, 1L);
			curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, appendHTMLChunk);
			curl_easy_setopt(curl, CURLOPT_WRITEDATA, &response_string);

			/* Perform the request, res will get the return code */
			CURLcode res = curl_easy_perform(curl);
			args->callback("[T:%d] URL fetch completed\n", self);
			/* Check for errors */
			if (res != CURLE_OK) {
				free(url);
				url = NULL;
				args->callback("____________BAD STUFF HAPPENED ErrorCode=%d!!\n", res);
				return (void*)res;
			}

			cJSON *json;
			json = cJSON_Parse(response_string.ptr);
			if (!json) {

				args->callback("CJSON Parsing didn't go well :(\n");
			}else{

				int vcjid = cJSON_GetObjectItem(json, "id")->valueint;
				cJSON *cjtx = cJSON_GetObjectItem(json, "text");
				cJSON *cjparent = cJSON_GetObjectItem(json, "parent");
				if (cjtx) {
					char* text = cjtx->valuestring;
					char* tx = malloc(strlen(text) + 1);
					strcpy(tx, text);
					ND* newnode = newCommentTreeNode(vcjid);
					int link_count = 0;
					listolinks hyperlinks;
				       	hyperlinks.links = NULL;	
					pthread_mutex_lock(&lock);
					newnode->text = extract_links(url_decode(dedup(tx)), &hyperlinks, &link_count);
					pthread_mutex_unlock(&lock);
					newnode->linkcount = link_count;
					if(link_count > 0){
						args->callback("Hyperlinks are %p, link #1 is %s\n", &hyperlinks, hyperlinks.links[0]);
						args->callback("Processed: %s\n", newnode->text);
						args->callback("Original: %s\n", tx);
					}
					newnode->links = hyperlinks.links;
					if (cjparent) {
						newnode->parentid = cjparent->valueint;
					}
					args->noderay[args->last_pushed_elem++] = newnode;
				}
				cJSON *cjkids = cJSON_GetObjectItem(json, "kids");
				if (cjkids) {
					int ikz = cJSON_GetArraySize(cjkids);
					int ki = 0;
					for (ki = 0; ki < ikz; ki++) {
						int kidid = cJSON_GetArrayItem(cjkids, ki)->valueint;
						args->callback("[T:%d]    Queuing up. ===> %d\n", self,  kidid);
						QAppendItem(args->queue, kidid,  &lock);
					}
				}

				cJSON_Delete(json);

				(*args->count)++;
			}

			int _sz = QSize(args->queue);
			args->callback("[Thread %u] Computing size \n", self);
			args->callback("[Thread %u] Iteration Complete [%d] \n", _sz, self);
			//free(response_string.ptr);
		}
		tries--;
		sleep(1);
		args->callback("[%u] Loop end... tries are now %d\n", self, tries);
	}
	args->callback("[Thread %u] Complete and exiting... \n", self);
	//curl_easy_cleanup(curl);
	return (void*)0;

}






int main(void)
{

	//int id = 11364550;
	int id = 11380251;

	curl_global_init(CURL_GLOBAL_ALL);
	pthread_t thread[NUMT];
	printf("Starting Queue curl test\n");
	pthread_mutex_init(&lock, NULL);
	printf("initialized locks\n");
	QH* queue = newQueue();
	int article_count = 0;
	struct thread_args* args = malloc(sizeof(struct thread_args));;
	args->count = &article_count;
	args->queue = queue;
	args->callback = &blip;
	args->tcount = 0;
	args->noderay = allocNodeArray(1000);
	args->last_pushed_elem = 0;
	//QAppendItem(queue, 11314597 , &lock);
	printf("Stuffing something onto the queue\n");

	QAppendItem(queue, id, &lock);
	printf("After appending... here is the queue..\n");
	//PrintQueue(queue);
	//return 0 ;
	printf("Starting up thread environment\n");
	struct timeval tv1, tv2;
	gettimeofday(&tv1, NULL);

	int i = 0;
	for (i = 0; i < NUMT; i++) {
		args->threadid = i;
		if (i == 0) {
			sleep(1);
		}

		pthread_create(&thread[i], NULL, downloadSingleURL, args);
	}
	for (i = 0; i < NUMT; i++) {
		pthread_join(thread[i], NULL);
	}
	printf("All Threads have finished... carrying on..");
	pthread_mutex_destroy(&lock);
	QueueEntireClear(queue);
	free(queue);
	curl_global_cleanup();
	gettimeofday(&tv2, NULL);
	double time_spent = (double)(tv2.tv_usec - tv1.tv_usec) / 1000000 +
			    (double)(tv2.tv_sec - tv1.tv_sec);
	printf("Time Spent was %lfs, articles download were %d \n ", time_spent, *(args->count));

	for (i = 0; i < 1000; i++) {
		if (args->noderay[i] != NULL) {
			printf("Debug Element.. elem @%d was %p, id=%d, par=%d, lc=%d,  tx=%s\n",
			       i,
			       args->noderay[i],
			       args->noderay[i]->id,
			       args->noderay[i]->parentid,
			       args->noderay[i]->linkcount,
			       substring(args->noderay[i]->text, 100));
		}
	}
	ND* root = newCommentTreeNode(id);
	buildCommentTree(root, args->noderay, 1000, 0);
	printf("Built comment tree - now dumping out tree\n");
	PrintTree(root, PRINT_ALL_TREE);
	return 0;
}
