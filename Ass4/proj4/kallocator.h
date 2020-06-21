#ifndef __KALLOCATOR_H__
#define __KALLOCATOR_H__
enum allocation_algorithm {FIRST_FIT, BEST_FIT, WORST_FIT};

struct nodeStruct {
	int size;
	void *ptr;
	struct nodeStruct *next;
};

/*
 * Allocate memory for a node of type struct nodeStruct and initialize
 * it with the value item. Return a pointer to the new node.
 */
struct nodeStruct* List_createNode(int size,void*ptr);

/*
 * Insert node at the head of the list.
 */
void List_insertHead (struct nodeStruct **headRef, struct nodeStruct *node);

/*
 * Insert node after the tail of the list.
 */
void List_insertTail (struct nodeStruct **headRef, struct nodeStruct *node);

/*
 * Count number of nodes in the list.
 * Return 0 if the list is empty, i.e., head == NULL
 */
int List_countNodes (struct nodeStruct *head);

/*
 * Return the first node holding the value item, return NULL if none found
 */
struct nodeStruct* List_findNode(struct nodeStruct *head, void*ptr);

/*
 * Delete node from the list and free memory allocated to it.
 * This function assumes that node has been properly set (by for example
 * calling List_findNode()) to a valid node in the list. If the list contains
 * only node, the head of the list should be set to NULL.
 */
void List_deleteNode (struct nodeStruct **headRef, struct nodeStruct *node);

void List_deleteAllnode(struct nodeStruct **headRef);



/*
 * Sort the list in ascending order based on the item field.
 * Any sorting algorithm is fine.
 */
void List_sort (struct nodeStruct **headRef);


void initialize_allocator(int _size, enum allocation_algorithm _aalgorithm);

void* kalloc(int _size);
void kfree(void* _ptr);
int available_memory();
void print_statistics();
int compact_allocation(void** _before, void** _after);
void destroy_allocator();

#endif
