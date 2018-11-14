#include "../lib/rts_taskset.h"
#include <sched.h>
#include <stdlib.h>
#include <stdint.h>
#include <sys/syscall.h>

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

struct sched_attr {
	uint32_t size;
	uint32_t sched_policy;
	uint64_t sched_flags;
	int32_t sched_nice;
	uint32_t sched_priority;
	uint64_t sched_runtime;
	uint64_t sched_deadline;
	uint64_t sched_period;
 };
 
void calc_prio(struct rts_plugin* this, struct rts_taskset* ts, struct rts_task* t) {
    return;
}
 
int sched_setattr(pid_t pid, const struct sched_attr *attr, unsigned int flags) {
    return syscall(__NR_sched_setattr, pid, attr, flags);
}

float test(struct rts_taskset* ts, struct rts_task* t, float free_budget) { 
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
    
    memset(attr, 0, sizeof(attr));
    attr.size = sizeof(attr);

    attr.sched_policy = SCHED_DEADLINE;
    attr.sched_runtime = t->wcet;
    attr.sched_deadline = t->deadline;
    attr.sched_period = t->period;

    return sched_setattr(t->tid, &attr, 0);
}

int deschedule(struct rts_task* t) {
    struct sched_param attr;
    
    attr.sched_priority = 0;
    return sched_setscheduler(t->tid, SCHED_OTHER, &attr);
}

void calc_budget(struct rts_plugin* this, struct rts_taskset* ts) {
    node_ptr* iterator;
    struct rts_task* t;
    
    for(iterator = ts->tasks->root; iterator != NULL; iterator = iterator->next) {
        t = (struct rts_task*)iterator->elem;
        
        if(t->plugin == this->type)
            this->used_budget += rts_task_utilization(t);
        
    }
}