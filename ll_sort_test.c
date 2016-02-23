
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
typedef struct nxn
{     
  struct nxn* next;      
  int n;
} nxn;     

int PrintTree(nxn* node)
{
	if (node == NULL) {
		printf("Whole tree was null");
		return -1;
	}
	nxn* tmp = node;
	while(tmp != NULL){
		printf("%p, %p, %d\n", tmp, tmp->next, tmp->n);
		tmp = tmp->next;
	}
	return 0;
}

void AppendNewNumber(nxn* head, int n){
	nxn* tmp = head;
	while(tmp->next != NULL){
		tmp = tmp->next;
	}

	nxn* node = malloc(sizeof(nxn));
	node->next = NULL;
	tmp->next = node;
	node->n = n;
		
}


nxn* findPreva(nxn* head, nxn* a){

		if(head == NULL ||  head == a){
			return NULL;
		}

		nxn* tmp = head;
		nxn* pre_a = NULL;
		while(tmp && tmp !=a){
			pre_a = tmp;
			tmp = tmp->next;
		}
		if(tmp != a){
			printf("Error.... \n");
		}

		return pre_a;


}

char* memlast(void *p){
	char* _tmp1 = malloc(14);
	char* _tmp = malloc(5);
	if(p == NULL){
		strcpy(_tmp, "NULL");
	}else{
		sprintf(_tmp1, "%p", p);
		memcpy(_tmp, &_tmp1[10], 4);
		_tmp[4] = '\0';
	}
	return _tmp;
	
}
void swap(nxn** head, nxn** a, nxn** b)
{

	if((*head) == NULL || (*a) == NULL || (*b) == NULL){
		printf("Nothing to swap");
		return;
	}
	printf("  A4 Performing swap...(%s,%s) (%s,%s)\n", memlast(*a), memlast(*b), memlast((*a)->next), memlast((*b)->next));
	// get previous nodes
	nxn* pre_a = findPreva(*head, *a);
	nxn* pre_b = findPreva(*head, *b);
	
	printf("  Previous nodes are (%s,%s)\n", memlast(pre_a), memlast(pre_b));


	if(pre_a){
		printf("  settingn a's next\n");
	       	pre_a->next = (*b); 
	}
	if(pre_b){
		printf("  settingn b's next to %s (%s,%s,%s)\n", memlast(*a), memlast(pre_b), memlast(pre_b->next), memlast(*a));
		 pre_b->next = (*a);
	}

	printf("  A5 after swap...(%s,%s) (%s,%s)\n", memlast(*a), memlast(*b), memlast((*a)->next), memlast((*b)->next));

	nxn* tmp = (*a)->next;
	(*a)->next = (*b)->next;
	(*b)->next = tmp;

			usleep(200 * 10000);


	if((*head)==(*a)){
	     printf("  G4 set head to a\n");
	     *head = *b;
	}
       if((*head)==(*b)){  
	     printf("  G4 set head to b\n");
	       *head = *a;
       }
}

void SortTree(nxn** head)
{
	int SwapCount = 0;
	int SortIterations = 0;
	while(SortIterations < 1000){
		SortIterations++;		
		nxn* tmp = *head;
		nxn* previous = NULL;
		// reset swap count;
		SwapCount=0;


		
		printf("Starting an iteration\n");
		while(tmp->next != NULL){
			printf("Checking....%p\n", tmp);
			if(tmp->next != NULL && tmp->n > tmp->next->n){
				printf("Swapping %d and %d (%p,%p)\n", tmp->n, tmp->next->n, tmp, tmp->next);
				swap(head, &tmp, &(tmp->next));
				printf("AFTER Swapping %d and %d (%p,%p)\n", tmp->n, tmp->next->n, tmp, tmp->next);
				SwapCount++;
				break;
			}else{
				printf("Not swapping\n");
			}
			previous = tmp;
			tmp = tmp->next;
			usleep(200 * 1000);
		}
		printf("Completing  an iteration\n");

		if(SwapCount == 0){
			break;
		}
	}

}

int main(void){

	printf("Starting demo test... \n");

	nxn* node = malloc(sizeof(nxn));
	node->n = 90;
	node->next = NULL;
	AppendNewNumber(node, 80);
	AppendNewNumber(node, 60);
	AppendNewNumber(node, 50);

	PrintTree(node);
	SortTree(&node);
	printf("Sorted out.. \n");
	PrintTree(node);


	printf("Done...\n");


}
