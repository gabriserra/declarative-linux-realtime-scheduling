/**
 * RT_TASKSET
 * A simple linked list implementation using standard C library
 */

#include "rt_taskset.h"

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
    list_init(&(ts->tasks));
}

/**
 * Check if the list is empty
 * 
 * [IN] struct list*: pointer to list
 * [OUT] int: 1 if empty, 0 otherwise
 */
int taskset_is_empty(struct rt_taskset* ts) {
    return list_is_empty(&(ts->tasks));
}

/**
 * Check the size of the list
 * 
 * [IN] struct list*: pointer to list
 * [OUT] int: 1 if empty, 0 otherwise
 */
int taskset_get_size(struct rt_taskset* ts) {
    return list_get_size(&(ts->tasks));
}

/**
 * Add the provided info str to the list
 * 
 * [IN] struct list*: pointer to list
 * [IN] char*: str to be added to the list
 * [OUT] void
 */
void taskset_add_top(struct rt_taskset* ts, struct rt_task* task) {
    list_add_top(&(ts->tasks), (void*) task);
}

/**
 * Add in a sorted way the element
 * 
 * [IN] struct list*: pointer to list
 * [IN] char*: str to be added to the list
 * [OUT] void
 */
void taskset_add_sorted_dl(struct rt_taskset* ts, struct rt_task* task) {
    list_add_sorted(&(ts->tasks), (void*) task, task_cmp_deadline);
}

/**
 * Remove the top element of the list
 * 
 * [IN] struct list*: pointer to list
 * [OUT] void
 */
void taskset_remove_top(struct rt_taskset* ts) {
    list_remove_top(&(ts->tasks));
}

/**
 * Return the pointer to info str of the top element of the list
 * 
 * [IN] struct list*: pointer to list
 * [OUT] char*: pointer to str info
 */
struct rt_task* taskset_get_top_task(struct rt_taskset* ts) {
    return list_get_top_elem(&(ts->tasks));
}

/**
 * Return the pointer to ith of the top element of the list
 * 
 * [IN] struct list*: pointer to list
 * [OUT] char*: pointer to str info
 */
struct rt_task* taskset_get_i_task(struct rt_taskset* ts, unsigned int i) {
    return list_get_i_elem(&(ts->tasks), i);
}