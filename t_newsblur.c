#include <string.h>
#include <unistd.h>
#include <stdio.h>
#include "downloader.h"


int main(int argc, char* argv[]){
    printf("Testing Downloader\n");

    /*
    void NewsBlurLogin(pthread_mutex_t lock,
                       struct thread_args* args,
                       unsigned int self,
                       CURL* curl,
                       char* username,
                       char* password
    )
    {
    */
    printf("count of args == %d\n", argc);
    int y = 1;
    char* username;
    char* password;
    if(argc > 2){
         username = strdup(argv[1]);
         password = strdup(argv[2]);
        printf("arg[%d] is {%s,%s}\n", argc, username, password);
    }
    pthread_t thread;
    struct thread_args* args = malloc(sizeof(struct thread_args));;

    pthread_mutex_t lock;

    pthread_mutex_init(&lock, NULL);

    CURL *curl;
    curl = curl_easy_init();
    if (!curl) {
        printf( "Exiting... Curl didn't seem to init correctly\n");
        return 1;
    }

    printf("Requesting from curl... \n");
    char* token = NewsBlurLogin(lock, args, 0, curl, username, password);
    NewsBlurFeeds(lock, args, 0, curl, token );
    int c = 1;
    int t = 0;
    int cn = -1;
    Story* story;
    while(cn != 0){
    	cn = NewsBlurArticles(lock, args, 0, curl, token, 6327282, c, story );
	t += cn;
	c++;
    }
    printf("Total Articles downloaded: %d\n", t);

}
