#include "simple.h"
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include <time.h>
#include "Articles.h"

#include <assert.h>


int Size(ArticleNode* head);
void print(ArticleNode* head, PrintStyle style);

void printl(int depth, char* prefix, ArticleNode* head);
void MoveArticleNode(ArticleNode** destref, ArticleNode** sourceRef);
void FrontBackSplit(ArticleNode* source, ArticleNode** fronthalf, ArticleNode** backhalf, int depth);



void append(ArticleNode* head, char* _new){
    while(head->next){ head=head->next;}
    ArticleNode* n = malloc(sizeof(ArticleNode));
    n->ArticleText = strdup(_new);
    n->next = NULL;
    head->next = n;
}

void AppendAll(ArticleNode* head,  char* CommaSepString1) {
    char* CommaSepString = strdup(CommaSepString1);
    char* token;
    token  = strtok(CommaSepString, ",");
    while(token != NULL){
        append(head, token);
        token = strtok(NULL, ",");
    }
    free(CommaSepString);
    free(token);
}

ArticleNode* NewArticleNode(){

    ArticleNode* n = malloc(sizeof(ArticleNode));
    n->next = NULL;
    return(n);
}

ArticleNode* NewArticleNodeWithText( const char* newText){

    ArticleNode* n = malloc(sizeof(ArticleNode));
    n->ArticleText = strdup(newText);
    n->next = NULL;
    return(n);
}

void print(ArticleNode* head, PrintStyle style){

    int sz = Size(head);
    while(head){
        if(style == CONTENTS_AND_POINTER) {
            printf("[%p]=>{%s}", head, head->ArticleText);
        }else{
            printf("{%s}", head->ArticleText);
        }
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

void printl(int depth, char* prefix, ArticleNode* head){

    int sz = Size(head);
    printf("[%*s]",(depth + 1) * 3, prefix);
    while(head){
        printf("[%p]=>{%s}", head, head->ArticleText);
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

int Size(ArticleNode* head){
    int i = 0;
    while(head->next){
        i++;
        head = head->next;
    }
    return i;
}

ArticleNode* SortedMerge(ArticleNode* a, ArticleNode* b){
    ArticleNode* result = NULL;
    if(a == NULL){
        return(b);
    }else if(b == NULL){
        return(a);
    }

    if(strcmp(a->ArticleText, b->ArticleText)){
        result = a;
        result->next = SortedMerge(a->next, b);
    } else{
        result = b;
        result->next = SortedMerge(a, b->next);
    }
    return(result);
}
ArticleNode* XSortedMerge(ArticleNode* a, ArticleNode* b){
    ArticleNode dummy;
    ArticleNode* tail = &dummy;
    while(1){
        if(a == NULL){
            tail->next = b;
        }else if(b == NULL){
            tail->next = a;
        }
        if(strcmp(a->ArticleText, b->ArticleText)){
            MoveArticleNode(&(tail->next), &a);
        }else{
            MoveArticleNode(&(tail->next), &b);
        }
    }

    return(dummy.next);
}


void MoveArticleNode(ArticleNode** destref, ArticleNode** sourceRef){
    ArticleNode* newArticleNode = *sourceRef;
    assert(newArticleNode != NULL);
    *sourceRef = newArticleNode->next;
    newArticleNode->next = *destref;
    *destref = newArticleNode;
}


void MergeSort(ArticleNode** headref, int depth){

    printf("%*s Merge sort @ depth %d\n", (depth + 1) * 3, "   ", depth);
    ArticleNode* head = *headref;
    ArticleNode* a;
    ArticleNode* b;
    if(head == NULL || head->next == NULL){
        return;
    }

    FrontBackSplit(head, &a, &b, depth);
    MergeSort(&a, depth + 1);
    MergeSort(&b, depth + 1);
    print(head, CONTENTS_AND_POINTER);
    printl(depth, "MergeSort", a);
    printl(depth, "MergeSort", b);
    *headref = SortedMerge(a, b);


}
void FrontBackSplit(ArticleNode* source, ArticleNode** fronthalf, ArticleNode** backhalf, int depth){
    ArticleNode* slow = source;
    ArticleNode* fast = source->next;
    while(fast != NULL){
        fast = fast->next;
        if(fast != NULL){
            fast = fast->next;
            slow = slow->next;
        }
    }

    *fronthalf = source;
    *backhalf = slow->next;
    printf("%*s[FBS][front=%p, back=%p]\n",(depth + 1) * 3, "   ",  *fronthalf, *backhalf);
    slow->next = NULL;
}


