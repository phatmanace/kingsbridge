
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
#include "yajl/yajl_tree.h"
#include "jansson.h"
#include "cjson/cJSON.h"

#define RUNS 10000


pthread_mutex_t lock;

struct thread_args {
	QH* queue;
	int* count;
	int threadid;
	int tcount;
	ND** noderay;
	int last_pushed_elem;
};

struct string {
	char *ptr;
	size_t len;
};





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

	unsigned int self = (unsigned int)pthread_self();

	printf("[%u] In Thread loop\n", self);
	CURL *curl;
	curl = curl_easy_init();
	if (!curl) {
		printf("Exiting... Curl didn't seem to init correctly\n");
		return (void*)0;
	}
	printf("[After init] In Thread loop\n");
	if (args->queue == NULL) {
		printf("Queue was null...");
	}
	;
	printf("[%u] Starting up thread while()..\n", self);
	int tries = 1;
	while (tries > 0) {
		while (QSize(args->queue) > 0) {
			struct string response_string;
			init_string(&response_string);

			printf("[%u] before pop queue size is now %d\n", self,  QSize(args->queue));
			int hnArticle = QpopItem(args->queue, &lock);
			if (hnArticle == -1) {
				printf("[%u] Blank Article... assume Queue was empty\n", self);
				continue;
			}
			char* url = malloc(sizeof(char) * 100);

			sprintf(url, "https://hacker-news.firebaseio.com/v0/item/%d.json", hnArticle);
			printf("[%u] %s z:%d\n", self,  url, QSize(args->queue));
			curl_easy_setopt(curl, CURLOPT_URL, url);
			curl_easy_setopt(curl, CURLOPT_TIMEOUT, 5L);
			curl_easy_setopt(curl, CURLOPT_FOLLOWLOCATION, 1L);
			curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, appendHTMLChunk);
			curl_easy_setopt(curl, CURLOPT_WRITEDATA, &response_string);

			/* Perform the request, res will get the return code */
			CURLcode res = curl_easy_perform(curl);
			printf("[%d] URL fetch completed\n", self);
			/* Check for errors */
			if (res != CURLE_OK) {
				free(url);
				url = NULL;
				printf("____________BAD STUFF HAPPENED ErrorCode=%d!!\n", res);
				return (void*)res;
			}

			cJSON *json;
			json = cJSON_Parse(response_string.ptr);
			if (!json) {

				printf("CJSON Parsing didn't go well :(\n");
			}else{

				int vcjid = cJSON_GetObjectItem(json, "id")->valueint;
				cJSON *cjtx = cJSON_GetObjectItem(json, "text");
				cJSON *cjparent = cJSON_GetObjectItem(json, "parent");
				if (cjtx) {
					char* text = cjtx->valuestring;
					char* tx = malloc(strlen(text) + 1);
					strcpy(tx, text);
					ND* newnode = newCommentTreeNode(vcjid);
					newnode->text = tx;
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
						printf("[%d]    Queuing up. ===> %d\n", self,  kidid);
						QAppendItem(args->queue, kidid,  &lock);
					}
				}

				cJSON_Delete(json);

				(*args->count)++;
			}

			int _sz = QSize(args->queue);
			printf("[Thread %u] Computing size \n", self);
			printf("[Thread %u] Iteration Complete [%d] \n", _sz, self);
			//free(response_string.ptr);
		}
		tries--;
		sleep(4);
		printf("[%u] Loop end... tries are now %d\n", self, tries);
	}
	printf("[Thread %u] Complete and exiting... \n", self);
	//curl_easy_cleanup(curl);
	return (void*)0;

}



#define NUMT 1

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
			printf("fb elem @%d was %p, id=%d, par=%d, tx=%s\n",
			       i,
			       args->noderay[i],
			       args->noderay[i]->id,
			       args->noderay[i]->parentid,
			       substring(args->noderay[i]->text, 10));
		}
	}
	ND* root = newCommentTreeNode(id);
	buildCommentTree(root, args->noderay, 1000, 0);
	printf("Built comment tree - now dumping out tree\n");
	PrintTree(root, PRINT_ALL_TREE);
	return 0;
}
