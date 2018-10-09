/**
 * RT_TASKSET
 * A simple linked list implementation using standard C library
 */

#include "rt_taskset.h"
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
void taskset_init(struct rt_taskset* ts) {
    ts->n = 0;
    ts->tasks = NULL;
}

/**
 * Check if the list is empty
 * 
 * [IN] struct list*: pointer to list
 * [OUT] int: 1 if empty, 0 otherwise
 */
int taskset_is_empty(struct rt_taskset* ts) {
    if(!ts->n)
        return 1;
    return 0;
}

/**
 * Add the provided info str to the list
 * 
 * [IN] struct list*: pointer to list
 * [IN] char*: str to be added to the list
 * [OUT] void
 */
void taskset_add_top(struct rt_taskset* ts, struct rt_task* task) {
    struct node* n = alloc(1, sizeof(struct node));

    n->next = ts->tasks;
    n->task = task;

    ts->n++;
    ts->tasks = n;
}

/**
 * Remove the top element of the list
 * 
 * [IN] struct list*: pointer to list
 * [OUT] void
 */
void taskset_remove_top(struct rt_taskset* ts) {
    struct node* n;

    if(taskset_is_empty(ts))
        return;
    
    n = ts->tasks;
    ts->tasks = ts->tasks->next;
    ts->n--;
    
    free(n);
}

/**
 * Return the pointer to info str of the top element of the list
 * 
 * [IN] struct list*: pointer to list
 * [OUT] char*: pointer to str info
 */
struct rt_task* taskset_get_top_task(struct rt_taskset* ts) {
    return ts->tasks->task;
}

/**
 * Return the pointer to ith of the top element of the list
 * 
 * [IN] struct list*: pointer to list
 * [OUT] char*: pointer to str info
 */
struct rt_task* taskset_get_i_task(struct rt_taskset* ts, unsigned int i) {
    int j;
    struct node* n;
    
    n = ts->tasks;
    for(j = 0; j < i; j++) {
        n = ts->tasks->next;
    }

    return n->task;
}