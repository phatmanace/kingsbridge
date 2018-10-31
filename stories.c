#include "stories.h"
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include <time.h>

#include <assert.h>


int StorySize(Story* head);
void printStory(Story* head);
void printl(int depth, char* prefix, Story* head);
void MoveStory(Story** destref, Story** sourceRef);
void FrontBackSplit(Story* source, Story** fronthalf, Story** backhalf, int depth);

Story* newStory(){

    Story* n = malloc(sizeof(Story));
    n->title = NULL;
    n->hash  = NULL;
    n->next  = NULL;
    return n;
}

void appendStory(Story* head, char*  _newhash, char* _newtitle){
    while(head->next){ head=head->next;}
    Story* n = malloc(sizeof(Story));
    n->title = strdup(_newtitle);
    n->hash = strdup(_newhash);
    n->next = NULL;
    head->next = n;
}

void printStory(Story* head){

    int y = 1;
    int sz = StorySize(head);
    while(head){
        printf("[%3d][%p]=>{%s}->{%s}",y,  head, head->title, head->hash);
        //printf("{%d}", head->data);

        if(head->next){
            head = head->next;
            printf("\n");
        }else{
            break;
        }
	y++;
    }
    printf("\n[total:%d]\n", sz);
}

void printl(int depth, char* prefix, Story* head){

    int y = 1;
    int sz = StorySize(head);
    printf("[%*s]",(depth + 1) * 3, prefix);
    while(head){
        printf("[%d][%p]=>{%s}", y, head, head->title);
        //printf("{%d}", head->data);

        if(head->next){
            head = head->next;
            printf("->");
        }else{
            break;
        }
	y++;
    }
    printf("[total:%d]\n", sz);
}

int StorySize(Story* head){
    int i = 0;
	
    if(head){
	i++;
    }else{
	return 0;
    }
    while(head->next){
        i++;
        head = head->next;
    }
    return i;
}


