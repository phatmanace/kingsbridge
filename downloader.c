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
   // printf("{%s} {%zu} {%zu}\n", buffer, nitems, size);
   char** token = (char**)userdata;
    char *tmp = strdup(buffer);
    tmp[strlen(tmp) - 2] = '\0';
   // printf("=> {%s}\n",  tmp);
    char *delim = ":";
    char* pos = strstr(tmp, delim);
    if(pos){
   	 //printf("Found at pos %s ==> %d\n",pos + 1, pos - tmp);
	    int header_name_len = pos - tmp;
	    int header_val_len = strlen(tmp) - 2 - header_name_len;
	    //printf("Print nl:%d and vl:%d\n", header_name_len, header_val_len);
	    char header_name[header_name_len];
	    char header_val[header_val_len];
	    int l = 0;
	    int p = 0;
	    while(tmp[l] != '\0'){
		//printf("=====> %c\n", tmp[l]);
 		if(l < header_name_len){
		    header_name[l] = tmp[l];
		    header_name[l+1] = '\0';
		}
		if(l == header_name_len){
			l += 2;
		};
 		if(l > header_name_len){
		    header_val[p] = tmp[l];
		    header_val[p+1] = '\0';
		    p++;
		}
		l++;
		(*tmp)++;
	    }
	    if(strcmp(header_name, "Set-Cookie") == 0) {
	    
            //printf("{name=%s}, {val=%s}\n", header_name, header_val);
            *token = strdup(header_val);
        }
	    
    }
    
    //char header_name[pos - tmp];
    //char header_value[strlen(tmp) - 2 - (pos - tmp)];
    free(tmp);
    return nitems * size;
}

char* NewsBlurLogin(pthread_mutex_t lock,
          struct thread_args* args,
          unsigned int self,
          CURL* curl,
          char* username,
          char* password
          )
{

    struct string response_string;
    init_string(&response_string);
    char *token;
    char* url = malloc(sizeof(char) * 100);
    char* poststring = malloc(sizeof(char) * 100);
    sprintf(poststring, "username=%s&password=%s", username, password);
    sprintf(url, "https://newsblur.com/api/login");
    curl_easy_setopt(curl, CURLOPT_URL, url);
    curl_easy_setopt(curl, CURLOPT_POSTFIELDS, poststring);
    curl_easy_setopt(curl, CURLOPT_USERAGENT, "Dark Secret Ninja/1.0");
    curl_easy_setopt(curl, CURLOPT_VERBOSE, 0L);
    curl_easy_setopt(curl, CURLOPT_HEADERFUNCTION, header_callback);
    curl_easy_setopt(curl, CURLOPT_HEADERDATA, &token);
    curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, BuildJSONString);
    curl_easy_setopt(curl, CURLOPT_WRITEDATA, &response_string);
    CURLcode res = curl_easy_perform(curl);
    long http_code = 0;
    curl_easy_getinfo (curl, CURLINFO_RESPONSE_CODE, &http_code);
    cJSON *json;
    json = cJSON_Parse(response_string.ptr);
    return token;
}
void NewsBlurFeeds(pthread_mutex_t lock,
          struct thread_args* args,
          unsigned int self,
          CURL* curl,
	  char* token
          )
{

    struct string response_string;
    init_string(&response_string);
    char* url = malloc(sizeof(char) * 100);
    char* tokenString = malloc(sizeof(char) * 100);
    sprintf(url, "https://newsblur.com/reader/feeds");
    curl_easy_setopt(curl, CURLOPT_URL, url);
    curl_easy_setopt(curl, CURLOPT_USERAGENT, "Dark Secret Ninja/1.0");
    curl_easy_setopt(curl, CURLOPT_VERBOSE, 0L);
    curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, BuildJSONString);
    curl_easy_setopt(curl, CURLOPT_WRITEDATA, &response_string);
    curl_easy_setopt(curl, CURLOPT_COOKIE, token);
    CURLcode res = curl_easy_perform(curl);
    long http_code = 0;
    curl_easy_getinfo (curl, CURLINFO_RESPONSE_CODE, &http_code);
    cJSON *json;
    json = cJSON_Parse(response_string.ptr);
    cJSON *current_element;
    char* current_key;
    printf("-----------------feeds-------------------\n");
    cJSON *feeds = cJSON_GetObjectItem(json, "feeds");
    cJSON_ArrayForEach(current_element, feeds )
    {
	    current_key = current_element->string;
	    if (current_key != NULL)
	    {   
		cJSON *nFeed = cJSON_GetObjectItem(feeds, current_key);
	        cJSON *feed_title = cJSON_GetObjectItem(nFeed, "feed_title");
		printf("[%8d]..%s\n", atoi(current_key), feed_title->valuestring );
	    }
    }
}

int NewsBlurArticles(pthread_mutex_t lock,
          struct thread_args* args,
          unsigned int self,
          CURL* curl,
	  char* token,
	  int hacker_news_feed_id,
	  int page,
	  Story** story
          )
{

    struct string response_string;
    init_string(&response_string);
    char* url = malloc(sizeof(char) * 100);
    char* tokenString = malloc(sizeof(char) * 100);
    sprintf(url, "https://newsblur.com/reader/feed/%d?read_filter=unread&page=%d", hacker_news_feed_id,page);
    curl_easy_setopt(curl, CURLOPT_URL, url);
    curl_easy_setopt(curl, CURLOPT_USERAGENT, "Dark Secret Ninja/1.0");
    curl_easy_setopt(curl, CURLOPT_VERBOSE, 0L);
    curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, BuildJSONString);
    curl_easy_setopt(curl, CURLOPT_WRITEDATA, &response_string);
    curl_easy_setopt(curl, CURLOPT_COOKIE, token);
    CURLcode res = curl_easy_perform(curl);
    long http_code = 0;
    curl_easy_getinfo (curl, CURLINFO_RESPONSE_CODE, &http_code);
    cJSON *json;
    json = cJSON_Parse(response_string.ptr);
    cJSON *current_element;
    char* current_key;
    printf("-----------------Stories-------------------\n");
    cJSON *feeds = cJSON_GetObjectItem(json, "stories");
    int story_count = 0;
    cJSON_ArrayForEach(current_element, feeds )
    {
	cJSON *story_title = cJSON_GetObjectItem(current_element, "story_title");
	cJSON *story_hash  = cJSON_GetObjectItem(current_element, "story_hash");
	printf("[%18s]..%s\n",  story_hash->valuestring, story_title->valuestring );
	appendStory(*story, story_hash->valuestring, story_title->valuestring );
	story_count++;
    }
	if((*story)->title == NULL){
		*story = (*story)->next;
	}
    return(story_count);
}
void DownloadArticleHeaders(pthread_mutex_t lock, struct thread_args* args, unsigned int self, CURL* curl) {
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
    printf("Return code was %d\n", res);

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
