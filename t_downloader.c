#include "curl/curl.h"
#include "downloader.h"
#include <string.h>
#include <unistd.h>
#define SMALL_HN_ARTICLE 18065567

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


void downloadSingle(CURL* curl){
   struct string response_string;
   init_string(&response_string);

   char* url = malloc(sizeof(char) * 100);

   sprintf(url, "https://hacker-news.firebaseio.com/v0/item/%d.json", SMALL_HN_ARTICLE);
   curl_easy_setopt(curl, CURLOPT_URL, url);
   curl_easy_setopt(curl, CURLOPT_TIMEOUT, 3L);
   curl_easy_setopt(curl, CURLOPT_VERBOSE, 1L);
   curl_easy_setopt(curl, CURLOPT_FOLLOWLOCATION, 1L);
   //curl_easy_setopt(curl, CURLOPT_WRITEDATA, &response_string);

   /* Perform the request, res will get the return code */
   CURLcode res = curl_easy_perform(curl);
   printf( " URL fetch completed" );
   /* Check for errors */
   if (res != CURLE_OK) {
      free(url);
      url = NULL;
      printf("Curl call failed: %d!!\n", res);
      return;
   }
   printf("Download was all ok\n");
   //free(response_string.ptr);

}

int main(void){

	CURL *curl;
	curl = curl_easy_init();
	if (!curl) {
		printf( "Exiting... Curl didn't seem to init correctly\n");
		return 1;
	}
	downloadSingle(curl);
}
