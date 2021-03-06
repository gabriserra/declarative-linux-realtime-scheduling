#include "rts_task.h"
#include "rts_utils.h"
#include <stdio.h>
#include <stdlib.h>
#include <sched.h>
#include <string.h>
#include <assert.h>

#define _GNU_SOURCE

//------------------------------------------
// PUBLIC: CREATE AND DESTROY FUNCTIONS
//------------------------------------------

// Instanciate and initialize a real time task structure
int rts_task_init(struct rts_task **tp, rsv_t id, clockid_t clk) {
    (*tp) = calloc(1, sizeof(struct rts_task));
    
    if((*tp) == NULL)
        return -1;
    
    (*tp)->id = id;
    (*tp)->clk = clk;
    
    return 0;
}

// Instanciate and initialize a real time task structure from another one
int rts_task_copy(struct rts_task *tp, struct rts_task *tp_copy) {
    tp = calloc(1, sizeof(struct rts_task));

    if (tp == NULL)
        return 0;

    memcpy(tp, tp_copy, sizeof(struct rts_task));
    return 1;
}

// Destroy a real time task structure
void rts_task_destroy(struct rts_task *tp) {
    free(tp);
}

//-----------------------------------------------
// PUBLIC: GETTER/SETTER
//------------------------------------------------

// Set the task worst case execution time
void rts_task_set_wcet(struct rts_task* tp, uint64_t wcet) {
    tp->wcet = wcet;
}

// Get the task worst case execution time
uint32_t rts_task_get_wcet(struct rts_task* tp) {
    return tp->wcet;
}

uint32_t rts_task_get_est_wcet(struct rts_task* tp) {
    return tp->wcet != 0 ? tp->wcet : rts_task_get_est_param(tp, EST_WCET); 
}

// Set the task period
void rts_task_set_period(struct rts_task* tp, uint32_t period) {
    tp->period = period;
}

// Get the task period
uint32_t rts_task_get_period(struct rts_task* tp) {
    return tp->period;
}

uint32_t rts_task_get_est_period(struct rts_task* tp) {
    return tp->period != 0 ? tp->period : rts_task_get_est_param(tp, EST_PERIOD);
}

// Set the relative deadline
void rts_task_set_deadline(struct rts_task* tp, uint32_t deadline) {
    tp->deadline = deadline;
}

// Get the relative deadline
uint32_t rts_task_get_deadline(struct rts_task* tp) {
    return tp->deadline;
}

uint32_t rts_task_get_est_deadline(struct rts_task* tp) {    
    if(tp->deadline != 0)
        return tp->deadline;
    else if(tp->period != 0)
        return tp->period;
    
    return rts_task_get_est_param(tp, EST_PERIOD);  
}

// ---
// Set priority and period of a task
// task_par* tp: pointer to tp data structure of the thread
// int period: desidered period of the thread (ms)
// int priority: desidered priority of the thread [1 low - 99 high]
// return: void
// ---
void set_priority(struct rts_task* tp, uint32_t priority) {
    if(priority < LOW_PRIO)
        tp->priority = LOW_PRIO;
    else if(priority > HIGH_PRIO)
        tp->priority = HIGH_PRIO;
    else
        tp->priority = priority;
}

// Get the priority
uint32_t get_priority(struct rts_task* tp) {
    return tp->priority;
}


int task_cmp_deadline(struct rts_task* tp1, struct rts_task* tp2) {
    if(tp1->deadline > tp2->deadline)
            return 1;
    else if(tp1->deadline < tp2->deadline)
            return -1;
    else
            return 0;
}

int task_cmp_period(struct rts_task* tp1, struct rts_task* tp2) {
    if(tp1->period > tp2->period)
        return 1;
    else if(tp1->period < tp2->period)
        return -1;
    else
        return 0;
}

int task_cmp_wcet(struct rts_task* tp1, struct rts_task* tp2) {
    if(tp1->wcet > tp2->wcet)
        return 1;
    else if(tp1->wcet < tp2->wcet)
        return -1;
    else
        return 0;
}

int task_cmp_priority(struct rts_task* tp1, struct rts_task* tp2) {
    if(tp1->priority > tp2->priority)
        return 1;
    else if(tp1->priority < tp2->priority)
        return -1;
    else
        return 0;
}

int task_cmp(struct rts_task* tp1, struct rts_task* tp2, enum PARAM p, int flag) {
	
    if(flag != ASC && flag != DSC)
        flag = ASC;

    switch (p) {
        case PERIOD:
            return flag * task_cmp_period(tp1, tp2);
        case WCET:
            return flag * task_cmp_wcet(tp1, tp2);
        case DEADLINE:
            return flag * task_cmp_deadline(tp1, tp2);
        case PRIORITY:
            return flag * task_cmp_priority(tp1, tp2);
        default:
            return flag * task_cmp_priority(tp1, tp2);
    }
}

int rts_task_get_est_param(struct rts_task* t, int FLAG) {
    return shatomic_get_value(&(t->est_param), FLAG);
}

void rts_task_update_util(struct rts_task* t) {
    uint32_t wcet;
    uint32_t period;
    
    wcet = rts_task_get_est_wcet(t);
    period = rts_task_get_est_period(t);
    
    t->util = (wcet / (float)period);    
}

float rts_task_get_util(struct rts_task* t) {
    return t->util;
}