#include "stories.h"
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include <time.h>

#include <assert.h>


int StorySize(Story* head);
void print(Story* head);
void print(Story* head);
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

void appendStory(Story* head, char*  _newtitle, char* _newhash){
    while(head->next){ head=head->next;}
    Story* n = malloc(sizeof(Story));
    n->title = strdup(_newtitle);
    n->hash = strdup(_newhash);
    n->next = NULL;
    head->next = n;
}

void print(Story* head){

    int sz = StorySize(head);
    while(head){
        printf("[%p]=>{%s}", head, head->title);
        //printf("{%d}", head->data);

        if(head->next){
            head = head->next;
            printf("->");
        }else{
            break;
        }
    }
    printf("[total:%d]\n", sz);
}

void printl(int depth, char* prefix, Story* head){

    int sz = StorySize(head);
    printf("[%*s]",(depth + 1) * 3, prefix);
    while(head){
        printf("[%p]=>{%s}", head, head->title);
        //printf("{%d}", head->data);

        if(head->next){
            head = head->next;
            printf("->");
        }else{
            break;
        }
    }
    printf("[total:%d]\n", sz);
}

int StorySize(Story* head){
    int i = 0;
    while(head->next){
        i++;
        head = head->next;
    }
    return i;
}


