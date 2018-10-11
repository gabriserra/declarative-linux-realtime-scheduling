/**
 * @file rt_taskset.c
 * @author Gabriele Serra
 * @date 11 Oct 2018
 * @brief Contains the implementation of a taskset (linked list of real time task) 
 *
 */

#include "rt_taskset.h"

// -----------------------------------------------------
// PRIVATE METHOD
// -----------------------------------------------------

/**
 * @internal
 *
 * The function is simply a wrapper to task compare function
 * 
 * @endinternal
 */
static int taskset_cmp(void* task1, void* task2) {
    return task_cmp_deadline((struct rt_task*) task1, (struct rt_task*) task2);
}

// -----------------------------------------------------
// PUBLIC METHOD
// -----------------------------------------------------

/**
 * @internal
 *
 * The function ensures that the taskset is in a consistent
 * state before to be used.
 * 
 * @endinternal
 */
void taskset_init(struct rt_taskset* ts) {
    list_ptr_init(&(ts->tasks));
}

/**
 * @internal
 *
 * Check if the taskset is empty. Return 0 if the taskset
 * contains at least one task, or 1 if the taskset is empty.
 * 
 * @endinternal
 */
int taskset_is_empty(struct rt_taskset* ts) {
    return list_ptr_is_empty(&(ts->tasks));
}

/**
 * @internal
 *
 * Return the size of the taskset. If the taskset
 * is empty, the function will return 0.
 * 
 * @endinternal
 */
int taskset_get_size(struct rt_taskset* ts) {
    return list_ptr_get_size(&(ts->tasks));
}

/**
 * @internal
 *
 * Add the provided element to the top of the taskset
 * 
 * @endinternal
 */
void taskset_add_top(struct rt_taskset* ts, struct rt_task* task) {
    list_ptr_add_top(&(ts->tasks), (void*) task);
}

/**
 * @internal
 *
 * The function allocate memory and add the task in the taskset 
 * in a sorted-way. Task with absolute deadline lower will be placed 
 * before task with absolute deadline greater. If there is another task
 * already in the taskset with equal deadline, the new task will be  
 * put after.
 * 
 * @endinternal
 */
void taskset_add_sorted_dl(struct rt_taskset* ts, struct rt_task* task) {
    list_ptr_add_sorted(&(ts->tasks), (void*) task, taskset_cmp_deadline);
}

/**
 * @internal
 *
 * Remove the first task of the taskset and frees memory.
 * If the taskset is empty, this function does nothing.
 * 
 * @endinternal
 */
void taskset_remove_top(struct rt_taskset* ts) {
    list_ptr_remove_top(&(ts->tasks));
}

/**
 * @internal
 *
 * Simply returns a pointer to the first task of the list.
 * If the taskset is empty, the function will return NULL.
 * 
 * @endinternal
 */
struct rt_task* taskset_get_top_task(struct rt_taskset* ts) {
    return list_ptr_get_top_elem(&(ts->tasks));
}

/**
 * @internal
 *
 * Return the pointer to the i-th task in the taskset. The taskset is 0-based. 
 * If "i" is greater or equal to the taskset size, the function returns NULL.
 * 
 * @endinternal
 */
struct rt_task* taskset_get_i_task(struct rt_taskset* ts, unsigned int i) {
    return list_ptr_get_i_elem(&(ts->tasks), i);
}

/**
 * @internal
 *
 * The function ensures that the taskset is in a consistent
 * state before to be used.
 * 
 * @endinternal
 */
/*
void* taskset_search_elem(struct rt_taskset* ts, struct rt_task* task) {
    return list_ptr_search_elem(&(ts->tasks), (void*) task, taskset_cmp_deadline);
}
*/

/**
 * @internal
 * 
 * Utilizes an in-place merge sort technique to sort the entire taskset.
 * Use one of WCET, PERIOD, DEADLINE, PRIORITY as enum value PARAM. If
 * the passed parameter is not valid, by default the taskset is sorted by
 * priority. Use ASC as flag to set an ascedent sorting or DSC on the
 * contrary. Other value are not valid, and in this case an ASC sorting
 * will be performed.
 * 
 * @endinternal
 */
void taskset_ptr_sort(struct rt_taskset* ts, enum PARAM p, int flag) {

}

#include <stdio.h>
#include <stdlib.h>

void print_list(struct list_ptr* l) {
    struct node_ptr* n;
    struct rt_task* t;
    
    for(n = l->root; n != NULL; n = n->next) {
        t = (struct rt_task*) n->elem;
        printf("%d -> ", get_deadline(t));
    }

    printf("\n");
}


int main() {
    struct rt_taskset ts;
    struct rt_task t1;
    struct rt_task t2;
    struct rt_task t3;


    taskset_init(&ts);
    
    rt_task_init(&t1, 55, 1);
    set_deadline(&t1, 10);

    rt_task_init(&t2, 56, 1);
    set_deadline(&t2, 5);

    rt_task_init(&t3, 57, 1);
    set_deadline(&t3, 12);
    
    taskset_add_sorted_dl(&ts, &t3);
    print_list(&(ts.tasks));
    taskset_add_sorted_dl(&ts, &t2);
    print_list(&(ts.tasks));
    taskset_add_sorted_dl(&ts, &t1);
    
    print_list(&(ts.tasks));

    printf("TEST ID: %d\n", get_deadline(taskset_get_i_task(&ts, 2)));
    //printf("TEST ID: %d\n", get_deadline(taskset_search_elem(&ts, &t1)));

    return 0;
}
