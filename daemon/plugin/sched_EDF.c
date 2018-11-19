#define _GNU_SOURCE

#include "../lib/rts_taskset.h"
#include "../lib/rts_plugin.h"
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
#include <sys/sysinfo.h>


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

int ts_recalc_utils(struct rts_plugin* this, struct rts_taskset* ts) {
    iterator_t iterator;
    struct rts_task* t;
    
    memset(this->util_used_percpu, 0, this->cpunum * sizeof(float));
    iterator = rts_taskset_iterator_init(ts);
    
    for(; iterator != NULL; iterator = iterator_get_next(iterator)) {
        t = rts_taskset_iterator_get_elem(iterator);
        
        if(t->pluginid == this->pluginid)
            this->util_used_percpu[t->cpu] += rts_task_get_util(t);
    }
    
    for(int i = 0; i < this->cpunum; i++)
        if(this->util_used_percpu[i] > 1)
            return -1;
    
    return 0;
}

void ts_recalc_prios(struct rts_plugin* this, struct rts_taskset* ts) {
    return;
}

int t_schedule(struct rts_task* t) {
    struct sched_attr attr;
    cpu_set_t my_set;

    CPU_ZERO(&my_set);
    CPU_SET(t->cpu, &my_set);

    if(sched_setaffinity(t->tid, sizeof(cpu_set_t), &my_set) < 0)
        return -1;
    
    memset(&attr, 0, sizeof(attr));
    attr.size = sizeof(attr);

    attr.sched_policy = SCHED_DEADLINE;
    attr.sched_runtime = MILLI_TO_NANO(rts_task_get_wcet(t));
    attr.sched_deadline = MILLI_TO_NANO(rts_task_get_deadline(t));
    attr.sched_period = MILLI_TO_NANO(rts_task_get_period(t));
    attr.sched_flags = 0;
    attr.sched_nice = 0;
    attr.sched_priority = 0;
        
    if(sched_setattr(t->tid, &attr, 0) < 0)
        return -1;
   
    return 0;
}

int t_deschedule(struct rts_task* t) {
    struct sched_param attr;
    cpu_set_t my_set;

    CPU_ZERO(&my_set);
    
    for(int i = 0; i < get_nprocs(); i++)
        CPU_SET(i, &my_set);
    
    if(sched_setaffinity(t->tid, sizeof(cpu_set_t), &my_set) < 0)
        return -1;
    
    attr.sched_priority = 0;
    
    if(sched_setscheduler(t->tid, SCHED_OTHER, &attr) < 0)
        return -1;
    
    return 0;
}

void t_add_to_utils(struct rts_plugin* this, struct rts_task* t) {
    this->util_used_percpu[t->cpu] += rts_task_get_util(t);
}

void t_remove_from_utils(struct rts_plugin* this, struct rts_task* t) {
    this->util_used_percpu[t->cpu] -= rts_task_get_util(t);
}

int t_recalc_util(struct rts_plugin* this, struct rts_task* t) {
    this->t_remove_from_utils(this, t);
    rts_task_update_util(t);
    this->t_add_to_utils(this, t);
    
    if(this->util_used_percpu[t->cpu] > 1)
        return -1;
    
    return 0;
}

void t_calc_prio(struct rts_plugin* this, struct rts_taskset* ts, struct rts_task* t) {
    return;
}

float t_test(struct rts_plugin* this, struct rts_taskset* ts, struct rts_task* t, float* free_utils) { 
    int required;
    int got;
    float task_util;
    int free_cpu = -1;
    
    task_util = rts_task_get_util(t);
    
    for(int i = 0; i < this->cpunum && free_cpu == -1; i++)
        if(task_util <= free_utils[i])
            free_cpu = i;
    
    if(free_cpu == -1)
        return 0;
    
    t->cpu = free_cpu;
    
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
