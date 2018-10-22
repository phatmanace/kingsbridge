#include "Articles.h"
#include <string.h>
#include <unistd.h>
#include <stdio.h>
#include "downloader.h"


int main(void){
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

    NewsBlurLogin(lock, args, 0, curl, "abc", "123");

}
