#include <stdio.h>
#include "list.h"
#include <stdbool.h>
#include <string.h>
#include <assert.h>
#include <stdlib.h>

/*
 * Allocate memory for a node of type struct nodeStruct and initialize
 * it with the value item. Return a pointer to the new node.
 */
struct nodeStruct* List_createNode(int val){
	struct nodeStruct* new = malloc(sizeof(struct nodeStruct));
	assert(new!=0);
	new->item=val;
	new->next=NULL;
	return new;
}

/*
 * Insert node at the head of the list.
 */
void List_insertHead (struct nodeStruct **headRef, struct nodeStruct *node){
	node->next=*headRef;	
	*headRef=node;
}
/*
 * Insert node after the tail of the list.
 */
void List_insertTail (struct nodeStruct **headRef, struct nodeStruct *node){
	struct nodeStruct* temp=*headRef;
	while(temp->next!=NULL)
		temp=temp->next;
	temp->next=node;
}
/*
 * Count number of nodes in the list.
 * Return 0 if the list is empty, i.e., head == NULL
 */
int List_countNodes (struct nodeStruct *head){
	int count=0;	
	struct nodeStruct* temp=head;
	while(temp!=NULL){
		count++;
		temp=temp->next;
	}
	return count;
}
/*
 * Return the first node holding the value item, return NULL if none found
 */
struct nodeStruct* List_findNode(struct nodeStruct *head, int val){
	struct nodeStruct* temp=head;
	while(temp!=NULL){
		if(temp->item==val)
			return temp;
		temp=temp->next;
	}
	return NULL;
}
/*
 * Delete node from the list and free memory allocated to it.
 * This function assumes that node has been properly set to a valid node 
 * in the list. For example, the client code may have found it by calling 
 * List_findNode(). If the list contains only one node, the head of the list 
 * should be set to NULL.
 */
void List_deleteNode (struct nodeStruct **headRef, struct nodeStruct *node){
	struct nodeStruct* temp=node;
	struct nodeStruct* before=*headRef;
	while(List_countNodes(*headRef) == 1){
		free(*headRef);
		return;
	}
	while(temp==before){
		*headRef=before->next;
		free(before);
		return;
	}
	while(temp!=before->next){
		before=before->next;
	}
	before->next=temp->next;
	free(temp);	
	return;
}
/*
 * Sort the list in ascending order based on the item field.
 * Any sorting algorithm is fine.
 */
void List_sort (struct nodeStruct **headRef){
	if(*headRef==NULL)
		return;
	struct nodeStruct* last=NULL;
	struct nodeStruct* curr=*headRef;
	bool flag=true;
	while(last!=curr->next&&flag){
		curr=*headRef;
		flag=false;
		while(curr->next!=last){
			if(curr->next->item<curr->item){
				int temp=curr->item;
				curr->item=curr->next->item;
				curr->next->item=temp;
				flag=true;
			}	
			curr=curr->next;
		}
		last=curr;
	}	
}

