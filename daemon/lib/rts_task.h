//-----------------------------------------------------------------------------
// RT_TASK_H: REPRESENT A REAL TIME TASK WITH ITS PARAM
//----------------------------------------------------------------------------- 

#ifndef RTS_TASK_H
#define RTS_TASK_H

#include <stdint.h>
#include <time.h>
#include <sys/types.h>
#include "rts_types.h"
#include "rts_plugin.h"

#define LOW_PRIO 	1			// lowest RT priority
#define HIGH_PRIO	99			// highest RT priority

enum PARAM {
	WCET,
	PERIOD,
	DEADLINE,
	PRIORITY
};

#define ASC 1
#define DSC -1

struct rts_task {
    rsv_t id;
    pid_t               ptid;		// parent tid
    pid_t               tid;		// thread/process id
    clockid_t           clk;            // type of clock to be used [REALTIME, MONOTONIC, ...]
    uint32_t            cpu;
    
    float               util;           // task CPU utilization [0.0 - 1.0]
    uint32_t		wcet;		// worst case ex time [microseconds]
    uint32_t 		period;		// period of task [millisecond]
    uint32_t 		deadline;	// relative deadline [millisecond]
    uint32_t 		priority;	// user priority of task
    uint32_t            schedprio;      // scheduling real prio [LOW_PRIO, HIGH_PRIO]

    
    int                 pluginid;       // if != NONE -> the scheduling alg
    struct shatomic     est_param;      // nactivation, wcet, period
};

//------------------------------------------
// PUBLIC: CREATE AND DESTROY FUNCTIONS
//------------------------------------------

// instantiate and initialize a real time task structure
int rts_task_init(struct rts_task **tp, rsv_t id, clockid_t clk);

// instantiate and initialize a real time task structure from another one
int rts_task_copy(struct rts_task *tp, struct rts_task *tp_copy);

// Destroy a real time task structure
void rts_task_destroy(struct rts_task *tp);

//-----------------------------------------------
// PUBLIC: GETTER/SETTER
//------------------------------------------------

// Set the task worst case execution time
void rts_task_set_wcet(struct rts_task* tp, uint64_t wcet);

// Get the task worst case execution time
uint32_t rts_task_get_wcet(struct rts_task* tp);

// Set the task period
void rts_task_set_period(struct rts_task* tp, uint32_t period);

// Get the task period
uint32_t rts_task_get_period(struct rts_task* tp);

// Set the relative deadline
void rts_task_set_deadline(struct rts_task* tp, uint32_t deadline);

// Get the relative deadline
uint32_t rts_task_get_deadline(struct rts_task* tp);

// Set the priority
void set_priority(struct rts_task* tp, uint32_t priority);

int task_cmp_deadline(struct rts_task* tp1, struct rts_task* tp2);

int task_cmp(struct rts_task* tp1, struct rts_task* tp2, enum PARAM p, int flag);

float rts_task_calc_budget(struct rts_task* t);

float rts_task_calc_rem_budget(struct rts_task* t);

int rts_task_get_est_param(struct rts_task* t, int FLAG);

void rts_task_update_util(struct rts_task* t);

float rts_task_get_util(struct rts_task* t);

#endif
