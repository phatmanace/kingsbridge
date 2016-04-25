
#include <stdio.h>
#include <curl/curl.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <regex.h>
#include "libconfig.h"
#include "hnarticle.h"
#include "hnfetcher.h"
#include "yajl/yajl_tree.h"
#include "zlog.h"
#include "downloader.h"



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


void FetchComments(zlog_category_t *c, int hnArticle)
{


	CURL *curl;
	CURLcode res;
	char errbuf[1024];
	char* url = malloc(sizeof(char) * 100);

	sprintf(url, "https://hacker-news.firebaseio.com/v0/item/%d.json", hnArticle);
	//zlog_info(c, "Fetching main article from %s", url);

	curl = curl_easy_init();
	if (curl) {
		struct string s;
		init_string(&s);
		curl_easy_setopt(curl, CURLOPT_URL, url);
		/* example.com is redirected, so we tell libcurl to follow redirection */
		curl_easy_setopt(curl, CURLOPT_FOLLOWLOCATION, 1L);
		curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, appendHTMLChunk);
		curl_easy_setopt(curl, CURLOPT_WRITEDATA, &s);

		/* Perform the request, res will get the return code */
		res = curl_easy_perform(curl);
		/* Check for errors */
		if (res != CURLE_OK) {
			zlog_error(c, "Failed to contact server for articles... %s", curl_easy_strerror(res));
			return;
		}

		yajl_val json_root = yajl_tree_parse((const char*)s.ptr, errbuf, sizeof(errbuf));
		if (json_root == NULL) {
			zlog_info(c, "Parser Errored");
		}
		int type = 1;
		{
			const char * path[] = { "type", (const char*)0 };
			yajl_val v = yajl_tree_get(json_root, path, yajl_t_string);
			if (v) {
				//zlog_info(c, "%s => %s", path[0], YAJL_GET_STRING(v));
				if (strcmp("story", YAJL_GET_STRING(v)) == 0) {
					zlog_info(c, "Saw Story");
					type = 2;
				}
			}else{
				zlog_info(c, "Failed to find Type path... \n");
			}
		}



		if (type == 2) {
			const char * path[] = { "title", (const char*)0 };
			yajl_val v = yajl_tree_get(json_root, path, yajl_t_string);
			if (v) {
				zlog_info(c, "%s => %s", path[0], YAJL_GET_STRING(v));
			}else{
				zlog_info(c, "Failed to find Story path... \n");
			}
		}
		if (type == 1) {
			const char * path[] = { "text", (const char*)0 };
			yajl_val v = yajl_tree_get(json_root, path, yajl_t_string);
			if (v) {
				zlog_info(c, "%s => %s", path[0], YAJL_GET_STRING(v));
			}else{
				zlog_info(c, "Failed to find Comment_text path... \n");
			}
		}
		{
			const char * path[] = { "kids", (const char*)0 };
			yajl_val v = yajl_tree_get(json_root, path, yajl_t_array);
			if (v) {
				size_t len = v->u.array.len;
				int i;
				for (i = 0; i < len; i++) {
					yajl_val _article = v->u.array.values[i];
					//zlog_info(c, "Comments...: %d", atoi(YAJL_GET_NUMBER(_article)));
					FetchComments(c, atoi(YAJL_GET_NUMBER(_article)));
				}
			}else{
				//zlog_info(c, "Failed to find Comment path... \n");
			}
		}

		/*

		   if (node && YAJL_IS_ARRAY(node)) {
		        size_t len = node->u.array.len;
		        int i;
		        for (i = 0; i < len; i++) {
		                yajl_val _article = node->u.array.values[i];
		                yajl_val title = _article->u.object.values[0];
		                yajl_val guid = _article->u.object.values[1];
		                yajl_val link = _article->u.object.values[2];
		                yajl_val saved = _article->u.object.values[3];
		                zlog_info(c, " ART..(%zu) %d [%zu] t:%s l:%s g:%s sa:%s", node->u.array.len, i, _article->u.object.len, YAJL_GET_STRING(title), YAJL_GET_STRING(link), YAJL_GET_STRING(guid), YAJL_GET_STRING(saved));
		                zlog_info(c, "-----------done Adding articles------------------\n");
		        }


		   }else{
		    zlog_info(c, "Didn't see what I expected on the parser");
		   }

		 */
		curl_easy_cleanup(curl);
	}

}
