/**
 * @file rt_taskset.h
 * @author Gabriele Serra
 * @date 11 Oct 2018
 * @brief Contains the interface of a taskset (linked list of real time task) 
 *
 * This file contains the interface of a simple implementation of a taskset
 * namely a list taskset real time task. This is useful to store in the taskset any custom
 * element, using the a cast to (void*). This implementation utilizes list_ptr.h, 
 * an implementation of linked list of void* element.
 */

#ifndef RT_TASKSET_H
#define RT_TASKSET_H

#include "../assets/list_ptr.h"
#include "rt_task.h"

// ---------------------------------------------
// DATA STRUCTURES
// ---------------------------------------------

/**
 * @brief Represent the taskset object
 * 
 * The structure rt_taskset contains a list_ptr. Inside
 * the taskset will be placed rt_tasks.
 */
struct rt_taskset {
    struct list_ptr tasks; /** the rt_task list  */
};

// ---------------------------------------------
// MAIN METHODS
// ---------------------------------------------

/**
 * @brief Initialize the taskset in order to be used
 * 
 * The function ensures that the taskset is in a consistent
 * state before to be used.
 * 
 * @param ts pointer to the taskset to be initialized
 */
void taskset_init(struct rt_taskset* ts);

/**
 * @brief Check if the taskset is empty
 * 
 * Check if the taskset is empty. Return 0 if the taskset
 * contains at least one task, or 1 if the taskset is empty.
 * 
 * @param ts pointer to taskset to be used to be used
 * @return 1 if empty, 0 otherwise
 */
int taskset_is_empty(struct rt_taskset* ts);

/**
 * @brief Return the size of the taskset
 * 
 * Return the size of the taskset. If the taskset
 * is empty, the function will return 0.
 * 
 * @param ts pointer to taskset to be used to be used
 * @return the size of the taskset or 0 if empty
 */
int taskset_get_size(struct rt_taskset* ts);

/**
 * @brief Add the provided element to the top of the taskset
 * 
 * Add the provided element to the top of the taskset
 * 
 * @param ts pointer to taskset to be used
 * @param task pointer to the task to be added to the taskset
 */
void taskset_add_top(struct rt_taskset* ts, struct rt_task* task);

/**
 * @brief Add the element to the taskset sorting by ASC deadline
 * 
 * The function allocate memory and add the task in the taskset 
 * in a sorted-way. Task with absolute deadline lower will be placed 
 * before task with absolute deadline greater. If there is another task
 * already in the taskset with equal deadline, the new task will be  
 * put after.
 * 
 * @param ts pointer to taskset to be used
 * @param task pointer to the task to be added to the taskset
 */
void taskset_add_sorted_dl(struct rt_taskset* ts, struct rt_task* task);

/**
 * @brief Remove the top element of the taskset
 * 
 * Remove the first task of the taskset and frees memory.
 * If the taskset is empty, this function does nothing.
 * 
 * @param ts: pointer to taskset to be used
 */
void taskset_remove_top(struct rt_taskset* ts);

/**
 * @brief Return a pointer to the first task of the list
 * 
 * Simply returns a pointer to the first task of the list.
 * If the taskset is empty, the function will return NULL.
 * 
 * @param ts pointer to taskset to be used
 * @return pointer to the first task of the list
 */
struct rt_task* taskset_get_top_task(struct rt_taskset* ts);

/**
 * @brief Return the pointer to the i-th task in the taskset
 * 
 * Return the pointer to the i-th task in the taskset. The taskset is 0-based. 
 * If "i" is greater or equal to the taskset size, the function returns NULL.
 * 
 * @param ts pointer to taskset to be used
 * @param i the index of the task to be retrivied
 * @return pointer to the element contained in the i-th node of the taskset
 */
struct rt_task* taskset_get_i_task(struct rt_taskset* ts, unsigned int i);

/**
 * @brief Sort (in place) the taskset
 * 
 * Utilizes an in-place merge sort technique to sort the entire taskset.
 * Use one of WCET, PERIOD, DEADLINE, PRIORITY as enum value PARAM. If
 * the passed parameter is not valid, by default the taskset is sorted by
 * priority. Use ASC as flag to set an ascedent sorting or DSC on the
 * contrary. Other value are not valid, and in this case an ASC sorting
 * will be performed.
 * 
 * @param l pointer to list to be used
 * @param p parameter of task to be used to establish the order
 * @param flag specify ASC for ascendent or DSC for descendent
 */
void taskset_ptr_sort(struct rt_taskset* ts, enum PARAM p, int flag);

#endif