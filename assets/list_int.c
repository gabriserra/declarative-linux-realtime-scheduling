/**
 * @file list_int.c
 * @author Gabriele Serra
 * @date 11 Oct 2018
 * @brief Contains the implementation of a linked list of integers 
 *
 */

#include "list_int.h"
#include <stdlib.h>
#include <syslog.h>
#include <string.h>
#include <errno.h>

// -----------------------------------------------------
// DEBUG METHOD
// -----------------------------------------------------

#ifdef DEBUG

#include <stdio.h>

/**
 * @internal
 *
 * The functions seek the list and prints each element
 * followed by an arrow ->. Remove #define DEBUG to hide
 * this function
 * 
 * @endinternal
 */
void print_list(struct list_int* l) {
    struct node_int* n;
    
    for(n = l->root; n != NULL; n = n->next) {
        printf("%d -> ", n->elem);
    }

    printf("\n");
}

#endif

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
void list_int_init(struct list_int* l) {
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
int list_int_is_empty(struct list_int* l) {
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
int list_int_get_size(struct list_int* l) {
    return l->n;
}

/**
 * @internal
 *
 * Add the provided element to the top of the list
 * 
 * @endinternal
 */
void list_int_add_top(struct list_int* l, int elem) {
    struct node_int* new = alloc(1, sizeof(struct node_int));

    new->next = l->root;
    new->elem = elem;

    l->n++;
    l->root = new;
}

/**
 * @internal
 *
 * The function allocate memory and add the element in the list 
 * in a sorted-way. If the element it's equal to another one,
 * will be put after. The cmpfun pointer function must be a function
 * that return a value greater than 1 is elem1 is greater than elem2,
 * -1 in the opposite case and 0 if elem1 and elem2 are equal. Use
 * int_cmp_asc or int_cmp_dsc if you don't want nothing special.
 * 
 * @endinternal
 */
void list_int_add_sorted(struct list_int* l, int elem, int (* cmpfun)(int elem, int lelem)) {
    struct node_int* seek;
    struct node_int* prec;
    struct node_int* new;

    if(list_is_empty(l) || cmpfun(elem, l->root->elem) < 0) {
        list_add_top(l, elem);
        return;
    }

    prec = l->root;
    new = alloc(1, sizeof(struct node_int));
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
void list_int_remove_top(struct list_int* l) {
    struct node_int* n;

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
int* list_int_get_top_elem(struct list_int* l) {
    if(list_is_empty(l))
        return NULL;
    
    return &(l->root->elem);
}

/**
 * @internal
 * 
 * Return the i-th element of the list. The list is 0-based. If "i" is
 * greater or equal to the list size, the function returns NULL.
 * 
 * @endinternal
 */
int* list_int_get_i_elem(struct list_int* l, unsigned int i) {
    int j;
    struct node_int* n;

    if(list_get_size(l) < i + 1)
        return NULL;

    n = l->root;
    
    for(j = 0; j < i; j++)
        n = n->next;

    return &(n->elem);
}

/**
 * @internal
 * 
 * Search the list and return a pointer to the first element
 * equal to "elem". If no equal element are found, the function returns
 * NULL 
 * 
 * @endinternal
 */
int* list_int_search_elem(struct list_int* l, int elem) {
    struct node_int* n;

    for(n = l->root; n != NULL; n = n->next)
        if(n->elem == elem)
            return &(n->elem);

    return NULL;
}

// ---------------------------------------------
// UTILITY
// ---------------------------------------------

/**
 * @internal
 *
 * Return 1 if elem1 is greater than elem2, -1 in the opposite case
 * or 0 if elem1 is equal to elem2. Can be used as compare function
 * in "list_add_sorted" to reach an ASC sorting.
 *  
 * @endinternal
 */
int int_cmp_asc(int elem1, int elem2) {
    if(elem1 > elem2)
        return 1;
    else if(elem1 < elem2)
        return -1;
    else
        return 0;
}

/**
 * @internal
 *
 * Return 1 if elem1 is greater than elem2, -1 in the opposite case
 * or 0 if elem1 is equal to elem2. Can be used as compare function
 * in "list_add_sorted" to reach an ASC sorting.
 *  
 * @endinternal
 */
int int_cmp_dsc(int elem1, int elem2) {
    if(elem1 > elem2)
        return -1;
    else if(elem1 < elem2)
        return 1;
    else
        return 0;
}

/*
int main() {
    struct list l;
    list_init(&l);
    list_add_top(&l, 5);
    list_add_top(&l, 5);
    list_add_top(&l, 5);
    print_list(&l);
    list_add_sorted(&l, 2, cmp_dsc);
    
    list_add_sorted(&l, 3, cmp_dsc);
    
    list_add_sorted(&l, 8, cmp_dsc);

    
    print_list(&l);
    list_remove_top(&l);
    printf("TOP ELEM: %d\n", *list_get_top_elem(&l));
    printf("3 ELEM: %d\n", *list_get_i_elem(&l, 2));
    return 0;
}
*/