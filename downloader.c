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

   args->callback(1, "[T:%u] before pop queue size is now %d\n", self,  QSize(args->queue));
   int hnArticle = QpopItem(args->queue, &lock);
   if (hnArticle == -1) {
      args->callback(1, "[%u] Queue was empty\n", self);
      return;
   }
   char* url = malloc(sizeof(char) * 100);

   sprintf(url, "https://hacker-news.firebaseio.com/v0/item/%d.json", hnArticle);
   args->callback(2, "[T:%u] %s Queue size:%-4d", self,  url, QSize(args->queue));
   curl_easy_setopt(curl, CURLOPT_URL, url);
   curl_easy_setopt(curl, CURLOPT_TIMEOUT, 5L);
   curl_easy_setopt(curl, CURLOPT_VERBOSE, 0L);
   curl_easy_setopt(curl, CURLOPT_FOLLOWLOCATION, 1L);
   curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, BuildJSONString);
   curl_easy_setopt(curl, CURLOPT_WRITEDATA, &response_string);

   /* Perform the request, res will get the return code */
   CURLcode res = curl_easy_perform(curl);
   args->callback(1, "[T:%d] URL fetch completed", self);
   /* Check for errors */
   if (res != CURLE_OK) {
      free(url);
      url = NULL;
      args->callback(1, "____________BAD STUFF HAPPENED ErrorCode=%d!!\n", res);
      return;
   }

   cJSON *json;
   json = cJSON_Parse(response_string.ptr);
   if (!json) {
	args->callback(1, "CJSON Parsing didn't go well :(\n");
   }else{
      int vcjid = cJSON_GetObjectItem(json, "id")->valueint;
      cJSON *cjtx     = cJSON_GetObjectItem(json, "text");
      cJSON *cscore   = cJSON_GetObjectItem(json, "score");
      cJSON *cjparent = cJSON_GetObjectItem(json, "parent");
      if (cscore){
		int score = cscore->valueint;
		args->callback(1, "Found a score %d", score);
      }else{
		args->callback(1, "Didn't find  a score");
      }
      if (cjtx) {
        char* text = cjtx->valuestring;
        char* tx = malloc(strlen(text) + 1);
        strcpy(tx, text);
        ND* newnode = nCmmt(vcjid);
        int link_count = 0;
        listolinks hyperlinks;
        hyperlinks.links = NULL;	
        pthread_mutex_lock(&lock);
        newnode->text = extract_links(url_decode(dedup(tx)), &hyperlinks, &link_count);
        pthread_mutex_unlock(&lock);
        newnode->linkcount = link_count;
        if(link_count > 0){
           args->callback(1, "Hyperlinks are %p, link #1 is %s", &hyperlinks, hyperlinks.links[0]);
           args->callback(1, "Processed: %s", newnode->text);
           args->callback(1, "Original: %s", tx);
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
            args->callback(1, "[T:%d]    Queuing up. ===> %d", self,  kidid);
            QAppendItem(args->queue, kidid,  &lock);
         }
      }
      cJSON_Delete(json);
      (*args->count)++;
   }
}

static size_t header_callback(char *buffer, size_t size,
                              size_t nitems, void *userdata)
{
    /* received header is nitems * size long in 'buffer' NOT ZERO TERMINATED */
    /* 'userdata' is set with CURLOPT_HEADERDATA */
    printf("I'm aheader\n");
    return nitems * size;
}

void NewsBlurLogin(pthread_mutex_t lock,
          struct thread_args* args,
          unsigned int self,
          CURL* curl,
          char* username,
          char* password
          )
{

    char* url = malloc(sizeof(char) * 100);
    char* poststring = malloc(sizeof(char) * 100);
    sprintf(poststring, "username=%s&password=%s", username, password);
    sprintf(url, "https://newsblur.com/api/login");
    curl_easy_setopt(curl, CURLOPT_URL, url);
    curl_easy_setopt(curl, CURLOPT_POSTFIELDS, poststring);
    curl_easy_setopt(curl, CURLOPT_VERBOSE, 1L);
    curl_easy_setopt(curl, CURLOPT_HEADERFUNCTION, header_callback);

    CURLcode res = curl_easy_perform(curl);
    printf("All done - login complete\n");
}

void DownloadArticleHeaders(pthread_mutex_t lock, struct thread_args* args, unsigned int self, CURL* curl){
    struct string response_string;
    init_string(&response_string);

    args->callback(1, "[T:%u] before pop queue size is now %d\n", self,  QSize(args->queue));
    int hnArticle = QpopItem(args->queue, &lock);
    if (hnArticle == -1) {
        args->callback(1, "[%u] Queue was empty\n", self);
        return;
    }
    char* url = malloc(sizeof(char) * 100);

    sprintf(url, "https://hacker-news.firebaseio.com/v0/item/%d.json", hnArticle);
    args->callback(2, "[T:%u] %s Queue size:%-4d", self,  url, QSize(args->queue));
    curl_easy_setopt(curl, CURLOPT_URL, url);
    curl_easy_setopt(curl, CURLOPT_TIMEOUT, 5L);
    curl_easy_setopt(curl, CURLOPT_VERBOSE, 0L);
    curl_easy_setopt(curl, CURLOPT_FOLLOWLOCATION, 1L);
    curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, BuildJSONString);
    curl_easy_setopt(curl, CURLOPT_WRITEDATA, &response_string);

    /* Perform the request, res will get the return code */
    CURLcode res = curl_easy_perform(curl);
    args->callback(1, "[T:%d] URL fetch completed", self);
    /* Check for errors */
    if (res != CURLE_OK) {
        free(url);
        url = NULL;
        args->callback(1, "____________BAD STUFF HAPPENED ErrorCode=%d!!\n", res);
        return;
    }

    cJSON *json;
    json = cJSON_Parse(response_string.ptr);
    if (!json) {
        args->callback(1, "CJSON Parsing didn't go well :(\n");
    }else{
        int vcjid = cJSON_GetObjectItem(json, "id")->valueint;
        cJSON *cjtx     = cJSON_GetObjectItem(json, "text");
        cJSON *cscore   = cJSON_GetObjectItem(json, "score");
        cJSON *cjparent = cJSON_GetObjectItem(json, "parent");
        if (cscore){
            int score = cscore->valueint;
            args->callback(1, "Found a score %d", score);
        }else{
            args->callback(1, "Didn't find  a score");
        }
        if (cjtx) {
            char* text = cjtx->valuestring;
            char* tx = malloc(strlen(text) + 1);
            strcpy(tx, text);
            ND* newnode = nCmmt(vcjid);
            int link_count = 0;
            listolinks hyperlinks;
            hyperlinks.links = NULL;
            pthread_mutex_lock(&lock);
            newnode->text = extract_links(url_decode(dedup(tx)), &hyperlinks, &link_count);
            pthread_mutex_unlock(&lock);
            newnode->linkcount = link_count;
            if(link_count > 0){
                args->callback(1, "Hyperlinks are %p, link #1 is %s", &hyperlinks, hyperlinks.links[0]);
                args->callback(1, "Processed: %s", newnode->text);
                args->callback(1, "Original: %s", tx);
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
                args->callback(1, "[T:%d]    Queuing up. ===> %d", self,  kidid);
                QAppendItem(args->queue, kidid,  &lock);
            }
        }
        cJSON_Delete(json);
        (*args->count)++;
    }
}