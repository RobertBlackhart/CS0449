#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "cdll.h"

/* just a convenience */
void fatal( char * msg )
{
	printf("%s\n",msg);
	exit(EXIT_FAILURE);
}


/* ----------------------------------------------------------------------------
	initList:

*/
void initList(CDLL *list, int (*compare)(void*, void*), void (*print)(void*, int),
			  void (*freeData)(void *))
{
	list->head = NULL;
	list->compare = compare;
	list->print = print;
	list->freeData = freeData;
}


/* ----------------------------------------------------------------------------
*/
void insertAtTail(CDLL *list, void *data)
{
	CDLL_NODE * node = malloc(sizeof(CDLL_NODE));
	
	if(!list->head)
	{
		//base case
		node->data = data;
		node->prev = node;
		node->next = node;
		list->head = node;
	}
	else
	{
		CDLL_NODE * oldTail = list->head->prev;
		node->data = data;
		node->prev = oldTail;
		node->next = list->head;
		list->head->prev = node;
		oldTail->next = node;
	}
}

/* ----------------------------------------------------------------------------
	deleteNode:

	You have  passed in the pointer to the node to be deleted.
	No need to iterate or search. Of course a check for a NULL pointer passed in
	would not hurt.
	Delete the deadNode then return the pointer to it's successor (if CW) or
	if you are going CCW then return pointer to the deadNode's predecessor.
	If deadnode was the last node then return NULL since there is no succ or pred.
*/
CDLL_NODE * deleteNode(CDLL *list, CDLL_NODE *deadNode, int direction )
{
	CDLL_NODE * prev = deadNode->prev;
	CDLL_NODE * next = deadNode->next;
	
	prev->next = next;
	next->prev = prev;
	
	if(deadNode == list->head)
	{
		list->head = next;
	}
	
	list->freeData(deadNode->data);
	free(deadNode);
	
	if(prev == deadNode && next == deadNode) //lastNode
	{
		return NULL;
	}
	if(direction == CLOCKWISE)
	{
		return next;
	}
	else
	{
		return prev;
	}
}



/* ----------------------------------------------------------------------------
	printList:

	Observe my solution executable to see how it should look
	You are really just writing the loop and calling the printData on each node
*/

void printList( CDLL list, int direction, int mode )
{
	direction++;
	CDLL_NODE * curr = list.head;
	list.print(curr->data,mode);
	curr = curr->next;
	while(curr != list.head)
	{
		list.print(curr->data,mode);
		curr = curr->next;
	}
	printf("\n");
}



/* ----------------------------------------------------------------------------
	searchList:

	Scan list until you find a node that contains the data value passed in.
	If found return that pointer - otherwise return NULL
*/
CDLL_NODE * searchList( CDLL list, void * target )
{
	CDLL_NODE * head = list.head;
	if(list.compare(target,head->data) == 0)
	{
		return head;
	}
	
	CDLL_NODE * curr = head->next;
	while(curr != head)
	{
		if(list.compare(target,curr->data) == 0)
		{
			return curr;
		}
		curr = curr->next;
	}
	
	return NULL;
}
