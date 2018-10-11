/**
 * @file list_ptr.c
 * @author Gabriele Serra
 * @date 11 Oct 2018
 * @brief Contains the interface of a linked list of void* 
 *
 */

#include "list_ptr.h"
#include <stdlib.h>
#include <syslog.h>
#include <string.h>
#include <errno.h>

// -----------------------------------------------------
// PRIVATE METHOD
// -----------------------------------------------------

/**
 * @internal
 *
 * The function allocate count * dimension memory in the heap.
 * If the system can not allocate memory, an error message
 * will be placed in the system log and the programs terminate.
 * 
 * @endinternal
 */
static void* alloc(int count, size_t dimension) {
	void* ret = calloc(count, dimension);

	// check if operation was performed
	if(ret != NULL)
		return ret;

	// print and exit
	syslog(LOG_ALERT, "The system is out of memory: %s", strerror(errno));
	exit(-1);
}

// -----------------------------------------------------
// PUBLIC METHOD
// -----------------------------------------------------

/**
 * @internal
 *
 * The function ensures that list is in a consistent
 * state before to be used. Put at 0 the number of element
 * and a pointer to NULL in the root.
 * 
 * @endinternal
 */
void list_init(struct list_ptr* l) {
    l->n = 0;
    l->root = NULL;
}

/**
 * @internal
 *
 * Check if the list is empty. Return 0 if the list
 * contains at least one node, or 1 if the list is empty.
 * 
 * @endinternal
 */
int list_is_empty(struct list_ptr* l) {
    if(!l->n)
        return 1;
    return 0;
}

/**
 * @internal
 *
 * Return the size of the list. If the list
 * is empty, the function will return 0.
 * 
 * @endinternal
 */
int list_get_size(struct list_ptr* l) {
    return l->n;
}

/**
 * @internal
 *
 * Add the provided element to the top of the list
 * 
 * @endinternal
 */
void list_add_top(struct list_ptr* l, void* elem) {
    struct node_ptr* n = alloc(1, sizeof(struct node_ptr));

    n->next = l->root;
    n->elem = elem;

    l->n++;
    l->root = n;
}

/**
 * @internal
 *
 * The function allocate memory and add the element in the list 
 * in a sorted-way. If the element it's equal to another one,
 * will be put after. The cmpfun pointer function must be a function
 * that return a value greater than 1 is elem1 is greater than elem2,
 * -1 in the opposite case and 0 if elem1 and elem2 are equal.
 * 
 * @endinternal
 */
void list_add_sorted(struct list_ptr* l, void* elem, int (* cmpfun)(void* elem1, void* elem2)) {
    struct node_ptr* seek;
    struct node_ptr* prec;
    struct node_ptr* new;

    if(list_is_empty(l) || cmpfun(elem, l->root->elem) < 0) {
        list_add_top(l, elem);
        return;
    }

    prec = l->root;
    new = alloc(1, sizeof(struct node_ptr));
    new->elem = elem;

    for(seek = l->root->next; seek != NULL && cmpfun(elem, seek->elem) > 0;) {
        prec = prec->next;
        seek = seek->next;
    }

    prec->next = new;
    new->next = seek;
    l->n++;
}

/**
 * @internal
 *
 * Remove the first node of the list and frees memory.
 * If the list is empty, this function does nothing.
 *  
 * @endinternal
 */
void list_remove_top(struct list_ptr* l) {
    struct node_ptr* n;

    if(list_is_empty(l))
        return;
    
    n = l->root;
    l->root = l->root->next;
    l->n--;
    
    free(n);
}

/**
 * @internal
 *
 * Simply returns a pointer to the element contained in the first node.
 * If the list is empty, the function will return NULL.
 *  
 * @endinternal
 */
void* list_get_top_elem(struct list_ptr* l) {
    if(list_is_empty(l))
        return NULL;

    return l->root->elem;
}

/**
 * @internal
 * 
 * Return the i-th element of the list. The list is 0-based. If "i" is
 * greater or equal to the list size, the function returns NULL.
 * 
 * @endinternal
 */
void* list_get_i_elem(struct list_ptr* l, unsigned int i) {
    int j;
    struct node_ptr* n;

    if(list_get_size(l) < i + 1)
        return NULL;
    
    n = l->root;

    for(j = 0; j < i; j++)
        n = n->next;

    return n->elem;
}

/**
 * @internal
 * 
 * Search the list and return a pointer to the first element
 * equal to "elem". If no equal element are found, the function returns
 * NULL. The cmpfun is must be a function that return 0 if elements are equal. 
 * 
 * @endinternal
 */
void* list_search_elem(struct list_ptr* l, void* elem, int (* cmpfun)(void* elem1, void* elem2)) {
    struct node_ptr* n;

    for(n = l->root; n != NULL; n = n->next)
        if(!cmpfun(n->elem, elem))
            return n->elem;

    return NULL;
}