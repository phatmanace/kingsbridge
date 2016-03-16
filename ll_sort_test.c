
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <time.h>
typedef struct nxn {
	struct nxn* next;
	int n;
} nxn;


/*
 *
 */

nxn * MergeLists(nxn *, nxn *); /* prototype */

/* sort a list using array of pointers to list       */
/* aList[i] == NULL or ptr to list with 2^i nodes    */

#define NUMLISTS 32             /* number of lists */
nxn * SortList(nxn *pList)
{
	nxn * aList[NUMLISTS]; /* array of lists */
	nxn * pNode;
	nxn * pNext;
	int i;

	if (pList == NULL)              /* check for empty list */
		return NULL;
	for (i = 0; i < NUMLISTS; i++)  /* init array */
		aList[i] = NULL;
	pNode = pList;                  /* merge nodes into array */
	while (pNode != NULL) {
		pNext = pNode->next;
		pNode->next = NULL;
		for (i = 0; (i < NUMLISTS) && (aList[i] != NULL); i++) {
			pNode = MergeLists(aList[i], pNode);
			aList[i] = NULL;
		}
		if (i == NUMLISTS) /* don't go beyond end of array */
			i--;
		aList[i] = pNode;
		pNode = pNext;
	}
	pNode = NULL;       /* merge array into one list */
	for (i = 0; i < NUMLISTS; i++)
		pNode = MergeLists(aList[i], pNode);
	return pNode;
}

/* merge two already sorted lists                    */
/* compare uses pSrc2 < pSrc1 to follow the STL rule */
/*   of only using < and not <=                      */
nxn * MergeLists(nxn *pSrc1, nxn *pSrc2)
{
	nxn *pDst = NULL;       /* destination head ptr */
	nxn **ppDst = &pDst;    /* ptr to head or prev->next */

	if (pSrc1 == NULL)
		return pSrc2;
	if (pSrc2 == NULL)
		return pSrc1;
	while (1) {
		if (pSrc2->n < pSrc1->n) { /* if src2 < src1 */
			*ppDst = pSrc2;
			pSrc2 = *(ppDst = &(pSrc2->next));
			if (pSrc2 == NULL) {
				*ppDst = pSrc1;
				break;
			}
		} else {                /* src1 <= src2 */
			*ppDst = pSrc1;
			pSrc1 = *(ppDst = &(pSrc1->next));
			if (pSrc1 == NULL) {
				*ppDst = pSrc2;
				break;
			}
		}
	}
	return pDst;
}

/*************************/

int PrintTree(nxn* node)
{
	if (node == NULL) {
		printf("Whole tree was null");
		return -1;
	}
	nxn* tmp = node;
	while (tmp != NULL) {
		//printf("%p, %p, %d\n", tmp, tmp->next, tmp->n);
		printf("        %10d\n",  tmp->n);
		tmp = tmp->next;
	}
	return 0;
}

void AppendNewNumber(nxn* head, int n)
{
	nxn* tmp = head;

	while (tmp->next != NULL) {
		tmp = tmp->next;
	}

	nxn* node = malloc(sizeof(nxn));
	node->next = NULL;
	tmp->next = node;
	node->n = n;

}


nxn* findPreva(nxn* head, nxn* a)
{

	if (head == NULL ||  head == a) {
		return NULL;
	}

	nxn* tmp = head;
	nxn* pre_a = NULL;
	while (tmp && tmp != a) {
		pre_a = tmp;
		tmp = tmp->next;
	}
	if (tmp != a) {
		printf("Error.... \n");
	}

	return pre_a;


}

char* memlast(void *p)
{
	char* _tmp1 = malloc(14);
	char* _tmp = malloc(5);

	if (p == NULL) {
		strcpy(_tmp, "NULL");
	}else{
		sprintf(_tmp1, "%p", p);
		memcpy(_tmp, &_tmp1[4], 4);
		_tmp[4] = '\0';
	}
	return _tmp;

}
void swap(nxn** head, nxn** a, nxn** b)
{

	if ((*head) == NULL || (*a) == NULL || (*b) == NULL) {
		printf("Nothing to swap");
		return;
	}
	//printf("  A4 Performing swap...(%s,%s) (%s,%s)\n", memlast(*a), memlast(*b), memlast((*a)->next), memlast((*b)->next));
	// get previous nodes
	nxn* pre_a = findPreva(*head, *a);
	nxn* pre_b = findPreva(*head, *b);

	//printf("  Previous nodes are (%s,%s)\n", memlast(pre_a), memlast(pre_b));

	nxn* tmp_b = (*b)->next;

	if ((*a) != (*head)) {
		pre_a->next = *b;
	}else{
		(*head) = (*b);
	}

	//printf("  G4 ...(%s,%s) (%s,%s)\n", memlast(*a), memlast(*b), memlast((*a)->next), memlast((*b)->next));

	if ((*a)->next == *b) {
		//printf("  H4 ...(%s,%s) (%s,%s) (T:%p)\n", memlast(*a), memlast(*b), memlast((*a)->next), memlast((*b)->next), tmp_b);

		(*b)->next = *a;
		(*a)->next = tmp_b;
	}else{
		// printf("  J4 ...(%s,%s) (%s,%s)\n", memlast(*a), memlast(*b), memlast((*a)->next), memlast((*b)->next));
		(*b)->next = (*a)->next;
		(*a)->next = tmp_b;
		pre_b->next = *a;
	}
	//printf("   Performed swap ok\n");

}

void SortTree(nxn** head)
{
	int SwapCount = 0;
	int SortIterations = 0;

	while (1) {
		SortIterations++;
		nxn* tmp = *head;

		// reset swap count;
		SwapCount = 0;



		//printf("Starting an iteration\n");
		//PrintTree(*head);
		while (tmp->next != NULL) {
			if (tmp->next != NULL && tmp->n > tmp->next->n) {
				swap(head, &tmp, &(tmp->next));
				SwapCount++;
				break;
			}
			tmp = tmp->next;
			//usleep(200 * 1000);
		}

		if (SwapCount == 0) {
			break;
		}
	}

}

int main(void)
{

	printf("Starting demo test... \n");
	srand(time(NULL));

	nxn* node = malloc(sizeof(nxn));
	node->n = 90;
	node->next = NULL;
	int i = 0;
	for (i = 0; i < 5000; i++) {
		AppendNewNumber(node, rand());
	}

	PrintTree(node);
	//	SortTree(&node);
	nxn* l = SortList(node);

	printf("Sorted out.. \n");
	PrintTree(l);


	printf("Done...\n");
	return -1;

}
