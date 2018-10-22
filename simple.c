#include "simple.h"
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include <time.h>

#include <assert.h>


int Size(Node* head);
void print(Node* head);
void print(Node* head);
void printl(int depth, char* prefix, Node* head);
void MoveNode(Node** destref, Node** sourceRef);
void FrontBackSplit(Node* source, Node** fronthalf, Node** backhalf, int depth);
void flip(Node** head){

    Node* n = malloc(sizeof(Node));
    n->data = 12;
    n->next = (Node*)0xCAFEBABE;
    free(*head);
    *head = n;
}

void append(Node* head, int _new){
    while(head->next){ head=head->next;}
    Node* n = malloc(sizeof(Node));
    n->data = _new;
    n->next = NULL;
    head->next = n;
}

void print(Node* head){

    int sz = Size(head);
    while(head){
        printf("[%p]=>{%d}", head, head->data);
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

void printl(int depth, char* prefix, Node* head){

    int sz = Size(head);
    printf("[%*s]",(depth + 1) * 3, prefix);
    while(head){
        printf("[%p]=>{%d}", head, head->data);
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

int Size(Node* head){
    int i = 0;
    while(head->next){
        i++;
        head = head->next;
    }
    return i;
}

Node* SortedMerge(Node* a, Node* b){
    Node* result = NULL;
    if(a == NULL){
        return(b);
    }else if(b == NULL){
        return(a);
    }

    if(a->data <= b->data){
        result = a;
        result->next = SortedMerge(a->next, b);
    } else{
        result = b;
        result->next = SortedMerge(a, b->next);
    }
    return(result);
}
Node* XSortedMerge(Node* a, Node* b){
    Node dummy;
    Node* tail = &dummy;
    while(1){
        if(a == NULL){
            tail->next = b;
        }else if(b == NULL){
            tail->next = a;
        }
        if(a->data < b->data){
            MoveNode(&(tail->next), &a);
        }else{
            MoveNode(&(tail->next), &b);
        }
    }

    return(dummy.next);
}


void MoveNode(Node** destref, Node** sourceRef){
    Node* newNode = *sourceRef;
    assert(newNode != NULL);
    *sourceRef = newNode->next;
    newNode->next = *destref;
    *destref = newNode;
}


void MergeSort(Node** headref, int depth){

    printf("%*s Merge sort @ depth %d\n", (depth + 1) * 3, "   ", depth);
    Node* head = *headref;
    Node* a;
    Node* b;
    if(head == NULL || head->next == NULL){
        return;
    }

    FrontBackSplit(head, &a, &b, depth);
    MergeSort(&a, depth + 1);
    MergeSort(&b, depth + 1);
    print(head);
    printl(depth, "MergeSort", a);
    printl(depth, "MergeSort", b);
    *headref = SortedMerge(a, b);


}
void FrontBackSplit(Node* source, Node** fronthalf, Node** backhalf, int depth){
    Node* slow = source;
    Node* fast = source->next;
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

int main(){
    time_t t;
    srand((unsigned) time(&t));
    printf("Starting simple test program\n");
    Node* n = malloc(sizeof(Node));
    n->data = 10;
    n->next = NULL;
    int i =0;
    while(i < 10) {
        append(n, rand() % 50);
        i++;
    }
    print(n);
    MergeSort(&n, 0);
    print(n);

    printf("All done\n");

}

