#include <string.h>
#include <unistd.h>
#include <stdio.h>
#include "downloader.h"
#include <libconfig.h>


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
    }else{
	printf("Usage [%s] <username> <password>\n", argv[0]);
	exit(1);
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
    if(token == NULL){
	    printf("Login to Newsblur failed\n");
	    exit(-1);
    }
    printf("Token was %s\n", token);
    int c = 1;
    int t = 0;
    int cn = -1;
    Story* story = newStory();
    while(cn != 0){
    	cn = NewsBlurArticles(lock, args, 0, curl, token, 6327282, c, &story );
	t += cn;
	c++;
    }
    config_t cfg;
    config_init(&cfg);
    config_setting_t *group = config_root_setting(&cfg);
    config_setting_t *root  = config_setting_add(group, "hndata", CONFIG_TYPE_GROUP);
    config_setting_t *ray   = config_setting_add(root, "articles", CONFIG_TYPE_ARRAY);
    Story *head = story;
    int i = 0;
    while(head != NULL && i++ < 10){
	config_setting_t *ray_item = config_setting_add(ray, NULL, CONFIG_TYPE_STRING);
	config_setting_set_string(ray_item, head->hash);
	head = head->next;
    }
    if(!config_write_file(&cfg, "/tmp/foo.cfg")){
	printf("Failed to write to file");	
    }
    printf("Config written to output file /tmp/foo.cfg\n");
    config_destroy(&cfg);
    //printStory(story);
    printf("Total Articles downloaded: %d (cf:%d)\n", t, StorySize(story));

}
