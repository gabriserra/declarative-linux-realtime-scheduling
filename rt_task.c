#include "rt_task.h"
#include <stdio.h>
#include <stdlib.h>
#include <sched.h>
#include <time.h>
#include <string.h>
#include <assert.h>

//---------------------------------
// PRIVATE: TIME UTILITY FUNCTIONS
//---------------------------------

// ---
// Copies a source time variable ts in a destination variable pointed by td
// timespec* td: pointer to destination timespec data structure
// timespec ts: source timespec data structure
// return: void
// ---
static void time_copy(struct timespec* td, struct timespec ts) {
    td->tv_sec  = ts.tv_sec;
    td->tv_nsec = ts.tv_nsec;
}

// ---
// Adds a value ms expressed in milliseconds to the time variable pointed by t
// timespec* t: pointer to timespec data structure
// int ms: value in milliseconds to add to t
// return: void
// ---
static void time_add_ms(struct timespec *t, int ms) {
	t->tv_sec += ms/1000;			 // convert ms to sec and add to sec
    t->tv_nsec += (ms%1000)*1000000; // convert and add the remainder to nsec
	
	// if nsec is greater than 10^9 means has reached 1 sec
	if (t->tv_nsec > 1000000000) { 
		t->tv_nsec -= 1000000000; 
		t->tv_sec += 1;
	}
}

// ---
// Compares time var t1, t2 and returns 0 if are equal, 1 if t1>t2, ‐1 if t1<t2
// timespec t1: first timespec data structure
// timespec t2: second timespec data structure
// return: int - 1 if t1 > t2, -1 if t1 < t2, 0 if they are equal
// ---
static int time_cmp(struct timespec t1, struct timespec t2) {
	// at first sec value is compared
	if (t1.tv_sec > t2.tv_sec) 
		return 1; 
	if (t1.tv_sec < t2.tv_sec) 
		return -1;
	//  at second nano sec value is compared
	if (t1.tv_nsec > t2.tv_nsec) 
		return 1; 
	if (t1.tv_nsec < t2.tv_nsec) 
		return -1; 
	return 0;
}

//------------------------------------------
// PUBLIC: CREATE AND DESTROY FUNCTIONS
//------------------------------------------

// Instanciate and initialize a real time task structure
int rt_task_init(struct rt_task *tp, pid_t tid, clockid_t clk) {
	tp->tid = tid;
	tp->clk = clk;
	return 1;
}

// Instanciate and initialize a real time task structure from another one
int rt_task_copy(struct rt_task *tp, struct rt_task *tp_copy) {
	tp = calloc(1, sizeof(struct rt_task));
	
	if (tp == NULL)
		return 0;

	memcpy(tp, tp_copy, sizeof(struct rt_task));
	return 1;
}

// Destroy a real time task structure
void rt_task_destroy(struct rt_task *tp) {
	free(tp);
}

//-----------------------------------------------
// PUBLIC: GETTER/SETTER
//------------------------------------------------

// Set the task worst case execution time
void set_wcet(struct rt_task* tp, uint64_t wcet) {
	tp->wcet = wcet;
}

// Get the task worst case execution time
uint64_t get_wcet(struct rt_task* tp) {
	return tp->wcet;
}

// Set the task period
void set_period(struct rt_task* tp, uint32_t period) {
	tp->period = period;
}

// Get the task period
uint32_t get_period(struct rt_task* tp) {
	return tp->period;
}

// Set the relative deadline
void set_deadline(struct rt_task* tp, uint32_t deadline) {
	tp->deadline = deadline;
}

// Get the relative deadline
uint32_t get_deadline(struct rt_task* tp) {
	return tp->deadline;
}

// ---
// Set priority and period of a task
// task_par* tp: pointer to tp data structure of the thread
// int period: desidered period of the thread (ms)
// int priority: desidered priority of the thread [1 low - 99 high]
// return: void
// ---
void set_priority(struct rt_task* tp, uint32_t priority) {
	if(priority < LOW_PRIO)
		tp->priority = LOW_PRIO;
	else if(priority > HIGH_PRIO)
		tp->priority = HIGH_PRIO;
	else
		tp->priority = priority;
}

// Get the priority
uint32_t get_priority(struct rt_task* tp) {
	return tp->priority;
}

// Get the deadline miss number
uint32_t get_dmiss(struct rt_task* tp) {
	return tp->dmiss;
}

// Get activation time (struct timespec)
void get_activation_time(struct rt_task* tp, struct timespec* at) {
	time_copy(at, tp->at);
}

// Get absolute deadline (struct timespec)
void get_deadline_abs(struct rt_task* tp, struct timespec* dl) {
	time_copy(dl, tp->dl);
}

//-------------------------------------
// PUBLIC: TASK PARAMATER MANAGEMENT
//--------------------------------------

// ---
// Reads the curr time and computes the next activ time and the deadline
// task_par* tp: pointer to tp data structure of the thread
// return: void
// ---
void calc_abs_value(struct rt_task* tp) {
	struct timespec t;
	
	// get current clock value
	clock_gettime(tp->clk, &t); 
	time_copy(&(tp->at), t); 
	time_copy(&(tp->dl), t);

	// adds period and deadline 
	time_add_ms(&(tp->at), tp->period); 
	time_add_ms(&(tp->dl), tp->deadline);
}

// ---
// Suspends the thread until the next activ and updates activ time and deadline
// task_par* tp: pointer to tp data structure of the thread
// return: void
// ---
void wait_for_period(struct rt_task* tp) {
	clock_nanosleep(tp->clk, TIMER_ABSTIME, &(tp->at), NULL);
	time_add_ms(&(tp->at), tp->period);
	time_add_ms(&(tp->dl), tp->period);
}

// ---
// Check if thread is in execution after deadline and return 1, otherwise 0.
// task_par* tp: pointer to tp data structure of the thread
// return: uint32_t - 1 if thread has executed after deadline, 0 otherwise
// ---
uint32_t deadline_miss(struct rt_task* tp) {
	struct timespec now;
	
	// get the clock time and compare to abs deadline
	clock_gettime(tp->clk, &now);

	if (time_cmp(now, tp->dl) > 0) { 
		tp->dmiss++;
		return 1; 
	}
	
	return 0;
}

int task_cmp_deadline(struct rt_task* tp1, struct rt_task* tp2) {
	if(tp1->deadline > tp2->deadline)
		return 1;
	else if(tp1->deadline < tp2->deadline)
		return -1;
	else
		return 0;
}