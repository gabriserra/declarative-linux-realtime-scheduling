#include "../lib/rts_taskset.h"
#include <sched.h>
#include <stdlib.h>
#include <stdint.h>
#include <unistd.h>
#include <string.h>
#include <sys/syscall.h>
#include <linux/unistd.h>
#include <linux/kernel.h>
#include <linux/types.h>
#include <stdio.h>
#include <errno.h>

#define _GNU_SOURCE

#ifdef __x86_64__
    #define __NR_sched_setattr		314
    #define __NR_sched_getattr		315
#endif

#ifdef __i386__
    #define __NR_sched_setattr		351
    #define __NR_sched_getattr		352
#endif

#ifdef __arm__
    #define __NR_sched_setattr		380
    #define __NR_sched_getattr		381
#endif

#define SCHED_DEADLINE	6
#define MILLI_TO_NANO(var) var * 1000 * 1000

struct sched_attr {
	__u32 size;

	__u32 sched_policy;
	__u64 sched_flags;

	/* SCHED_NORMAL, SCHED_BATCH */
	__s32 sched_nice;

	/* SCHED_FIFO, SCHED_RR */
	__u32 sched_priority;

	/* SCHED_DEADLINE (nsec) */
	__u64 sched_runtime;
	__u64 sched_deadline;
        __u64 sched_period;
 };
  
int sched_setattr(pid_t pid, const struct sched_attr *attr, unsigned int flags) {
    return syscall(__NR_sched_setattr, pid, attr, flags);
}

extern float test(struct rts_plugin* this, struct rts_taskset* ts, struct rts_task* t, float free_budget) { 
    int required;
    int got;
    
    if(free_budget < rts_task_utilization(t))
        return 0;
    
    got = 0;
    required = 3;
    
    if(t->wcet != 0)
        got++;
    if(t->period != 0)
        got++;
    if(t->deadline != 0)
        got++;
    
    return (got/ (float)required);
}

int schedule(struct rts_task* t) {
    struct sched_attr attr;
    
    memset(&attr, 0, sizeof(attr));
    attr.size = sizeof(attr);

    attr.sched_policy = SCHED_DEADLINE;
    attr.sched_runtime = MILLI_TO_NANO(rts_task_get_wcet(t));
    attr.sched_deadline = MILLI_TO_NANO(rts_task_get_deadline(t));
    attr.sched_period = MILLI_TO_NANO(rts_task_get_period(t));
    attr.sched_flags = 0;
    attr.sched_nice = 0;
    attr.sched_priority = 0;
    
    return sched_setattr(t->tid, &attr, 0);
}

int deschedule(struct rts_task* t) {
    struct sched_param attr;
    
    attr.sched_priority = 0;
    return sched_setscheduler(t->tid, SCHED_OTHER, &attr);
}

void calc_prio(struct rts_plugin* this, struct rts_taskset* ts, struct rts_task* t) {
    return;
}

void calc_budget(struct rts_plugin* this, struct rts_taskset* ts) {
    iterator_t iterator;
    struct rts_task* t;
    
    iterator = rts_taskset_iterator_init(ts);
    
    for(; iterator != NULL; iterator = rts_taskset_iterator_get_next(iterator)) {
        t = rts_taskset_iterator_get_elem(iterator);
        
        if(t->plugin == this->type)
            this->used_budget += rts_task_utilization(t);
        
    }
}