#include "downloader.h"
#include "curl/curl.h"
#include "cjson/cJSON.h"
#include <string.h>
#include <pthread.h>
#include <unistd.h>

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

size_t BuildJSONString(void *ptr, size_t size, size_t memb, struct string *s)
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

void downloadURL(pthread_mutex_t lock, struct thread_args* args, unsigned int self, CURL* curl){


			struct string response_string;
			init_string(&response_string);

			args->callback("[T:%u] before pop queue size is now %d\n", self,  QSize(args->queue));
			int hnArticle = QpopItem(args->queue, &lock);
			if (hnArticle == -1) {
				args->callback("[%u] Queue was empty\n", self);
				return;
			}
			char* url = malloc(sizeof(char) * 100);

			sprintf(url, "https://hacker-news.firebaseio.com/v0/item/%d.json", hnArticle);
			args->callback("[T:%u] %s z:%d\n", self,  url, QSize(args->queue));
			printf("[T:%u] %s z:%d\n", self,  url, QSize(args->queue));
			curl_easy_setopt(curl, CURLOPT_URL, url);
			curl_easy_setopt(curl, CURLOPT_TIMEOUT, 5L);
			curl_easy_setopt(curl, CURLOPT_FOLLOWLOCATION, 1L);
			curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, BuildJSONString);
			curl_easy_setopt(curl, CURLOPT_WRITEDATA, &response_string);

			/* Perform the request, res will get the return code */
			CURLcode res = curl_easy_perform(curl);
			args->callback("[T:%d] URL fetch completed\n", self);
			/* Check for errors */
			if (res != CURLE_OK) {
				free(url);
				url = NULL;
				args->callback("____________BAD STUFF HAPPENED ErrorCode=%d!!\n", res);
				return;
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
}
