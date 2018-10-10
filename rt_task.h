//-----------------------------------------------------------------------------
// RT_TASK_H: REPRESENT A REAL TIME TASK WITH ITS PARAM
//----------------------------------------------------------------------------- 

#ifndef RT_TASK_H
#define RT_TASK_H

#include <stdint.h>
#include <sys/types.h>
#include <time.h>

#define LOW_PRIO 	1			// lowest RT priority
#define HIGH_PRIO	99			// highest RT priority

struct rt_task {
	pid_t			tid;		// thread/process id
	clockid_t 		clk;		// type of clock to be used [REALTIME, MONOTONIC, ...]
	uint64_t		wcet;		// worst case ex time [microseconds]
	uint32_t 		period;		// period of task [millisecond]
	uint32_t 		deadline;	// relative deadline [millisecond]
	uint32_t 		priority;	// priority of task [LOW_PRIO, HIGH_PRIO] 
	uint32_t 		dmiss;		// num of deadline misses
	struct timespec at;			// next activation time 
	struct timespec dl; 		// absolute deadline
};

//------------------------------------------
// PUBLIC: CREATE AND DESTROY FUNCTIONS
//------------------------------------------

// Instanciate and initialize a real time task structure
int rt_task_init(struct rt_task *tp, pid_t tid, clockid_t clk);

// Instanciate and initialize a real time task structure from another one
int rt_task_copy(struct rt_task *tp, struct rt_task *tp_copy);

// Destroy a real time task structure
void rt_task_destroy(struct rt_task *tp);

//-----------------------------------------------
// PUBLIC: GETTER/SETTER
//------------------------------------------------

// Set the task worst case execution time
void set_wcet(struct rt_task* tp, uint64_t wcet);

// Get the task worst case execution time
uint64_t get_wcet(struct rt_task* tp);

// Set the task period
void set_period(struct rt_task* tp, uint32_t period);

// Get the task period
uint32_t get_period(struct rt_task* tp);

// Set the relative deadline
void set_deadline(struct rt_task* tp, uint32_t deadline);

// Get the relative deadline
uint32_t get_deadline(struct rt_task* tp);

// Set the priority
void set_priority(struct rt_task* tp, uint32_t priority);

// Get the priority
uint32_t get_priority(struct rt_task* tp);

// Get the deadline miss number
uint32_t get_dmiss(struct rt_task* tp);

// Get activation time (struct timespec)
void get_activation_time(struct rt_task* tp, struct timespec* at);

// Get absolute deadline (struct timespec)
void get_deadline_abs(struct rt_task* tp, struct timespec* dl);

//-------------------------------------
// PUBLIC: TASK PARAMATER MANAGEMENT
//--------------------------------------

// Reads the curr time and computes the next activ time and the deadline
void calc_abs_value(struct rt_task* tp);

// Suspends the thread until the next activ and updates activ time and deadline
void wait_for_period(struct rt_task* tp);

// Check if thread is in execution after deadline and return 1, otherwise 0.
uint32_t deadline_miss(struct rt_task* tp);

int task_cmp_deadline(struct rt_task* tp1, struct rt_task* tp2);

#endif
