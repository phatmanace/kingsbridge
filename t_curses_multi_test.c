
#include <stdio.h>
#include <sys/time.h>
#include <stdlib.h>
#include <string.h>
#include <pthread.h>
#include <unistd.h>

#include "queue.h"
#include "comment_tree.h"
#include "curl/curl.h"
#include "yajl/yajl_tree.h"
#include "jansson.h"

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
	if(!curl){
		printf("Exiting... Curl didn't seem to init correctly\n");
		return (void*)0;
	}
	printf("[After init] In Thread loop\n");
	if (args->queue == NULL) {
		printf("Queue was null...");
	}
	;
	printf("[%u] Starting up thread..", self);
	int tries = 3;
	while (tries > 0) {
		while (QSize(args->queue) > 0) {
			struct string s;
			init_string(&s);

			printf("[%u] before pop queue size is now %d\n", self,  QSize(args->queue));
			int hnArticle = QpopItem(args->queue, &lock);
			if(hnArticle == -1){
				printf("[%u] Blank Article... assume Queue was empty\n", self);
				continue;
			}
			char* url = malloc(sizeof(char) * 100);
			sprintf(url, "https://hacker-news.firebaseio.com/v0/item/%d.json", hnArticle);
			printf("[%u] Popped off  ... %s - queue size is now %d\n", self,  url, QSize(args->queue));
			curl_easy_setopt(curl, CURLOPT_URL, url);
			curl_easy_setopt(curl, CURLOPT_FOLLOWLOCATION, 1L);
			curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, appendHTMLChunk);
			curl_easy_setopt(curl, CURLOPT_WRITEDATA, &s);

			/* Perform the request, res will get the return code */
			CURLcode res = curl_easy_perform(curl);
			/* Check for errors */
			if (res != CURLE_OK) {
				free(url);
				url = NULL;
				printf("____________BAD STUFF HAPPENED ErrorCode=%d!!\n", res);
				return (void*)0;
			}
			json_t *root;
			json_error_t error;
			json_t *id, *jtx, *jkids;
			root = json_loads(s.ptr, 0, &error);
			if(!root){
				printf("JANSSONG PARSE didn't go well :( ");
				printf("Text was |%s|\n", s.ptr);
			}
			id = json_object_get(root, "id");
			jtx = json_object_get(root, "text");
			if(json_string_value(jtx) != NULL && id != NULL){
				char* tx = malloc(strlen(json_string_value(jtx)));
				strcpy(tx, json_string_value(jtx));
				printf("COMMENT_ID %lf which is %d\n ", json_number_value(id), (int)json_number_value(id));
				ND* newnode = newCommentTreeNode((int)json_number_value(id));
				newnode->text = tx;
				args->noderay[args->last_pushed_elem++] = newnode;
			}

			jkids = json_object_get(root, "kids");
			if(jkids && json_is_array(jkids)){
				int i = 0;
				for(i = 0; i < json_array_size(jkids); i++){
					json_t *kid;
					kid = json_array_get(jkids, i);
					printf(" [%u] (enqueing child) ID=%d\n", self, (int)json_integer_value(kid));	
					QAppendItem(args->queue, json_integer_value(kid), &lock);
					printf(" [%u] (complete_enqueing child) ID=%d\n", self, (int)json_integer_value(kid));	
				}
					
			}
			(*args->count)++;
			

			json_decref(root);
			
			free(url);
			int _sz = QSize(args->queue);
			url = NULL;
			printf("[Thread %u] Computing size \n", self);
			printf("[Thread %u] Iteration Complete [%d] \n", _sz, self);
		}
		tries--;
		sleep(4);
		printf("[%u] Loop end... tries are now %d\n", self, tries);
	}
	printf("[Thread %u] Complete \n", self);
	curl_easy_cleanup(curl);
	return (void*)0;

}



#define NUMT 2

int main(void)
{

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
	QAppendItem(queue, 3472928, &lock);
	printf("After appending... here is the queue..\n");
	//PrintQueue(queue);
	//return 0 ;
	printf("Starting up thread environment\n");
	struct timeval tv1, tv2;
	gettimeofday(&tv1, NULL);

	int i = 0;
	for (i = 0; i < NUMT; i++) {
		args->threadid = i;
		if(i == 0){
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
			printf("fb elem @%d was %p, id=%d, tx=%s\n",
			       i,
			       args->noderay[i],
			       args->noderay[i]->id,
			       args->noderay[i]->text);
		}
	}
	return 0;
}
