//modify provided sol of a1
//borrow idea of adding member variable void *block into linked list struct from https://embeddedartistry.com/blog/2017/2/9/implementing-malloc-first-fit-free-list


#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include "kallocator.h"
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>

static _Bool doSinglePassOnSort(struct nodeStruct **headRef);
static void swapElements(struct nodeStruct **previous, struct nodeStruct *nodeA, struct nodeStruct *b);

/*
 * Allocate memory for a node of type struct nodeStruct and initialize
 * it with the value item. Return a pointer to the new node.
 */
struct nodeStruct* List_createNode(int size,void* ptr)
{
	struct nodeStruct *pNode = malloc(sizeof(struct nodeStruct));
	if (pNode != NULL) {
		pNode->size = size;
		pNode->ptr=ptr;
	}
	return pNode;
}

/*
 * Insert node at the head of the list.
 */
void List_insertHead (struct nodeStruct **headRef, struct nodeStruct *node)
{
	node->next = *headRef;
	*headRef = node;
}

/*
 * Insert node after the tail of the list.
 */
void List_insertTail (struct nodeStruct **headRef, struct nodeStruct *node)
{
	node->next = NULL;

	// Handle empty list
	if (*headRef == NULL) {
		*headRef = node;
	}
	else {
		// Find the tail and insert node
		struct nodeStruct *current = *headRef;
		while (current->next != NULL) {
			current = current->next;
		}
		current->next = node;
	}
}

/*
 * Count number of nodes in the list.
 * Return 0 if the list is empty, i.e., head == NULL
 */
int List_countNodes (struct nodeStruct *head)
{
	int count = 0;
	struct nodeStruct *current = head;
	while (current != NULL) {
		current = current->next;
		count++;
	}
	return count;
}

/*
 * Return the first node holding the value item, return NULL if none found
 */
struct nodeStruct* List_findNode(struct nodeStruct *head, void*ptr)
{
	struct nodeStruct *current = head;
	while (current != NULL) {
		if (current->ptr == ptr) {
			return current;
		}
		current = current->next;
	}
	return NULL;
}

/*
 * Delete node from the list and free memory allocated to it.
 * This function assumes that node has been properly set (by for example
 * calling List_findNode()) to a valid node in the list. If the list contains
 * only node, the head of the list should be set to NULL.
 */
void List_deleteNode (struct nodeStruct **headRef, struct nodeStruct *node)
{
	assert(headRef != NULL);
	assert(*headRef != NULL);

	// Is it the first element?
	if (*headRef == node) {
		*headRef = node->next;
	}
	else {
		// Find the previous node:
		struct nodeStruct *previous = *headRef;
		while (previous->next != node) {
			previous = previous->next;
			assert(previous != NULL);
		}

		// Unlink node:
		assert(previous->next == node);
		previous->next = node->next;
	}

	// Free memory:
	free(node);
}

void List_deleteAllnode(struct nodeStruct **headRef){
	assert(headRef != NULL);
	assert(*headRef != NULL);
	for(struct nodeStruct* temp = *headRef;temp!=NULL;){
		*headRef = temp->next;
        	free(temp);
		temp = *headRef;
	}
	*headRef = NULL;
}
/*
 * Sort the list in ascending order based on the item field.
 * Any sorting algorithm is fine.
 */
void List_sort (struct nodeStruct **headRef)
{
	while (doSinglePassOnSort(headRef)) {
		// Do nothing: work done in loop condition.
	}
}
static _Bool doSinglePassOnSort(struct nodeStruct **headRef)
{
	_Bool didSwap = false;
	while (*headRef != NULL) {
		struct nodeStruct *nodeA = *headRef;
		// If we don't have 2 remaining elements, nothing to swap.
		if (nodeA->next == NULL) {
			break;
		}
		struct nodeStruct *nodeB = nodeA->next;

		// Swap needed?
		if (nodeA->ptr > nodeB->ptr){
			swapElements(headRef, nodeA, nodeB);
			didSwap = true;
		}

		// Advance to next elements
		headRef = &((*headRef)->next);
	}
	return didSwap;
}
static void swapElements(struct nodeStruct **previous,
		struct nodeStruct *nodeA,
		struct nodeStruct *nodeB)
{
	*previous = nodeB;
	nodeA->next = nodeB->next;
	nodeB->next = nodeA;
}

struct KAllocator {
	enum allocation_algorithm aalgorithm;
	int size;
	void* memory;
    	// Some other data members you want, 
    	// such as lists to record allocated/free memory

	struct nodeStruct *allocatedBlocks;
	struct nodeStruct *freeBlocks;
};

struct KAllocator kallocator;


void initialize_allocator(int _size, enum allocation_algorithm _aalgorithm) {
	assert(_size > 0);
	kallocator.aalgorithm = _aalgorithm;
	kallocator.size = _size;
	kallocator.memory = malloc((size_t)kallocator.size);
    	// Add some other initialization 
	kallocator.allocatedBlocks=NULL;
	kallocator.freeBlocks=List_createNode(kallocator.size,kallocator.memory);
}

void destroy_allocator() {
	free(kallocator.memory);

    // free other dynamic allocated memory to avoid memory leak
	if(kallocator.freeBlocks!=NULL)
		List_deleteAllnode(&kallocator.freeBlocks);
	if(kallocator.allocatedBlocks!=NULL)
		List_deleteAllnode(&kallocator.allocatedBlocks);
}

void* kalloc(int _size) {
  	void* ptr = NULL;
    // Allocate memory from kallocator.memory 
    // ptr = address of allocated memory
	if(kallocator.aalgorithm==0){
		struct nodeStruct* current=kallocator.freeBlocks;
		//find the first block that satisfies first fit
		while(current!=NULL){
			if(current->size>=_size){
				break;
			}
			current=current->next;
		}

		if(current!=NULL){
			//set ptr
			ptr=current->ptr;

			//update current block
			current->size-=_size;
			current->ptr=(void*)(current->ptr+_size);

			//remove current from freeBlocks if it is full
			if(current->size==0)
				List_deleteNode(&kallocator.freeBlocks, current);
		}
		if(ptr!=NULL){
			//use new to hold the latest allocated node
			struct nodeStruct *new=List_createNode(_size,ptr);		
			List_insertTail(&kallocator.allocatedBlocks, new);
		}
	
	}
	else if(kallocator.aalgorithm==1){
		struct nodeStruct*temp=kallocator.freeBlocks;
		struct nodeStruct*current=NULL;
		int diff=kallocator.size;//minDiff is large enough

		//find the first block that satisfies best fit
		while(temp!=NULL){
			int tempDiff=temp->size-_size;
			if(tempDiff>=0&&tempDiff<diff){
				diff=tempDiff;
				current=temp;
			}
			temp=temp->next;
		}

		if(current!=NULL){
			//set ptr
			ptr=current->ptr;

			//update current block
			current->size-=_size;
			current->ptr=(void*)(current->ptr+_size);

			//remove current from freeBlocks if it is full
			if(current->size==0)
				List_deleteNode(&kallocator.freeBlocks, current);
		}
		if(ptr!=NULL){
			//use new to hold the latest allocated node
			struct nodeStruct *new=List_createNode(_size,ptr);		
			List_insertTail(&kallocator.allocatedBlocks, new);
		}

	}
	else if(kallocator.aalgorithm==2){
		struct nodeStruct*temp=kallocator.freeBlocks;
		struct nodeStruct*current=NULL;
		int diff=-1024;//maxDiff is small enough

		//find the first block that satisfies best fit
		while(temp!=NULL){
			int tempDiff=temp->size-_size;
			if(tempDiff>=0&&tempDiff>diff){
				diff=tempDiff;
				current=temp;
			}
			temp=temp->next;
		}

		if(current!=NULL){
			//set ptr
			ptr=current->ptr;

			//update current block
			current->size-=_size;
			current->ptr=(void*)(current->ptr+_size);

			//remove current from freeBlocks if it is full
			if(current->size==0)
				List_deleteNode(&kallocator.freeBlocks, current);
		}
		if(ptr!=NULL){
			//use new to hold the latest allocated node
			struct nodeStruct *new=List_createNode(_size,ptr);		
			List_insertTail(&kallocator.allocatedBlocks, new);
		}
	}
	return ptr;
}

void kfree(void* _ptr) {
	assert(_ptr != NULL);
	struct nodeStruct* current=List_findNode(kallocator.allocatedBlocks,_ptr);

	if(current!=NULL){
		struct nodeStruct* new=List_createNode(current->size,_ptr);

		List_insertHead(&kallocator.freeBlocks, new);
		List_deleteNode(&kallocator.allocatedBlocks, current);

		List_sort (&kallocator.freeBlocks);

		struct nodeStruct* current = kallocator.freeBlocks;

		while(current!=NULL){
			if(current->next!=NULL){
				if (current->ptr + current->size == current->next->ptr) {
                	    		current->size = current->size + current->next->size;
                	    		List_deleteNode(&(kallocator.freeBlocks), current->next);
                		}
			}	
			current=current->next;	
		}
	}
}

int compact_allocation(void** _before, void** _after) {
	int compacted_size = 0;
	
	// compact allocated memory
	// update _before, _after and compacted_size
	List_sort(&kallocator.allocatedBlocks);
	struct nodeStruct *temp = kallocator.allocatedBlocks;

	if(temp != NULL) {
        	if (temp->ptr != kallocator.memory) {
        	    (_before[compacted_size]) = temp->ptr;
        	    temp->ptr = kallocator.memory;
        	    (_after[compacted_size]) = temp->ptr;
        	    memmove(_after[compacted_size], _before[compacted_size], temp->size);
        	    compacted_size++;
        	}

    	}

    	while (temp != NULL) {
        	if (temp->next != NULL) {
        		if ((temp->next->ptr != temp->ptr + temp->size)) {
                		(_before[compacted_size]) = temp->next->ptr;
         		     	temp->next->ptr = temp->ptr + temp->size;
               			compacted_size++;
            		}
        	}
        	temp = temp->next;
	}
	int ava_mem = available_memory();
        int alloc_mem = kallocator.size - ava_mem;
	while (kallocator.freeBlocks != NULL) {
		List_deleteNode(&(kallocator.freeBlocks), kallocator.freeBlocks);
	}
	struct nodeStruct *newN = List_createNode(ava_mem, kallocator.memory + alloc_mem);
	List_insertTail(&(kallocator.freeBlocks), newN);
	return compacted_size;
}

int available_memory() {
	int available_memory_size = 0;
	// Calculate available memory size
	struct nodeStruct* temp=kallocator.freeBlocks;
	while(temp!=NULL){
		available_memory_size+=temp->size;
		temp=temp->next;
	}
	return available_memory_size;
}

void print_statistics() {
	int allocated_size = 0;
	int allocated_chunks = 0;
	int free_size = 0;
	int free_chunks = 0;
	int smallest_free_chunk_size = kallocator.size;
	int largest_free_chunk_size=0;

	// Calculate the statistics
	struct nodeStruct* temp=kallocator.allocatedBlocks;
	while(temp!=NULL){
		allocated_chunks++;
		allocated_size+=temp->size;
		temp=temp->next;
	}
	//allocated_chunks = List_countNodes(kallocator.allocatedBlocks);
	temp=kallocator.freeBlocks;
	while(temp!=NULL){
		free_chunks++;
		free_size+=temp->size;
		if(smallest_free_chunk_size>temp->size)
			smallest_free_chunk_size=temp->size;
		if(largest_free_chunk_size<temp->size)
			largest_free_chunk_size=temp->size;	
		temp=temp->next;
	}
	//free_chunks = List_countNodes(kallocator.freeBlocks);

	printf("Allocated size = %d\n", allocated_size);
	printf("Allocated chunks = %d\n", allocated_chunks);
	printf("Free size = %d\n", free_size);
	printf("Free chunks = %d\n", free_chunks);
	printf("Largest free chunk size = %d\n", largest_free_chunk_size);
	printf("Smallest free chunk size = %d\n", smallest_free_chunk_size);
}



