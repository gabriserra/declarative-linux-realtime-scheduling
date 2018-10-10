/**
 * LIST
 * A simple linked list implementation using standard C library
 */

#include "list.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// -----------------------------------------------------
// PRIVATE METHOD
// -----------------------------------------------------

/**
 * Allocate a block of memory and fill with 0s. Exit in case of error 
 * 	
 * [IN] int: count of element to be allocated
 * [OUT] size_t: dimension of each element
 */
static void* alloc(int count, size_t dimension) {
	void* ret = calloc(count, dimension);

	// check if operation was performed
	if(ret != NULL)
		return ret;

	// Print and exit
	printf("Out of memory. Please restart your machine.");
	exit(-1);
}

// -----------------------------------------------------
// PUBLIC METHOD
// -----------------------------------------------------

/**
 * Initialize the list in order to be used
 * 
 * [IN] struct list*: pointer to list to be initialized
 * [OUT] void
 */
void list_init(struct list* l) {
    l->n = 0;
    l->root = NULL;
}

/**
 * Check if the list is empty
 * 
 * [IN] struct list*: pointer to list
 * [OUT] int: 1 if empty, 0 otherwise
 */
int list_is_empty(struct list* l) {
    if(!l->n)
        return 1;
    return 0;
}

/**
 * Check the size of the list
 * 
 * [IN] struct list*: pointer to list
 * [OUT] int: 1 if empty, 0 otherwise
 */
int list_get_size(struct list* l) {
    return l->n;
}

/**
 * Add the provided info str to the list
 * 
 * [IN] struct list*: pointer to list
 * [IN] char*: str to be added to the list
 * [OUT] void
 */
void list_add_top(struct list* l, void* elem) {
    struct node* n = alloc(1, sizeof(struct node));

    n->next = l->root;
    n->elem = elem;

    l->n++;
    l->root = n;
}

/**
 * Add in a sorted way the element
 * 
 * [IN] struct list*: pointer to list
 * [IN] char*: str to be added to the list
 * [OUT] void
 */
void list_add_sorted(struct list* l, void* elem, int (* cmpfun)(void* elem1, void* elem2)) {
    struct node* seek;
    struct node* new;

    if(list_is_empty(l)) {
        list_add_top(l, elem);
        return;
    }

    new = alloc(1, sizeof(struct node));
    new->elem = elem;

    for(seek = l->root; seek != NULL; seek = seek->next)
        if(sortfun(elem, seek->elem) > 0)
            break;

    new->next = seek->next;
    seek->next = new;
}

/**
 * Remove the top element of the list
 * 
 * [IN] struct list*: pointer to list
 * [OUT] void
 */
void list_remove_top(struct list* l) {
    struct node* n;

    if(list_is_empty(l))
        return;
    
    n = l->root;
    l->root = l->root->next;
    l->n--;
    
    free(n);
}

/**
 * Return the pointer to info str of the top element of the list
 * 
 * [IN] struct list*: pointer to list
 * [OUT] char*: pointer to str info
 */
void* list_get_top_elem(struct list* l) {
    return l->root->elem;
}

/**
 * Return the pointer to ith of the top element of the list
 * 
 * [IN] struct list*: pointer to list
 * [OUT] char*: pointer to str info
 */
void* list_get_i_elem(struct list* l, unsigned int i) {
    int j;
    struct node* n;
    
    n = l->root;
    for(j = 0; j < i; j++) {
        n = n->next;
    }

    return n->elem;
}