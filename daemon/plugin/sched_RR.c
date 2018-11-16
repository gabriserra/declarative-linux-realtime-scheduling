#define _GNU_SOURCE

#include "../lib/rts_taskset.h"
#include <sched.h>
#include <stdlib.h>
#include <string.h>

#define MIN(val1, val2) (val1 < val2 ? val1 : val2)
#define MAX(val1, val2) (val1 > val2 ? val1 : val2)

void get_prio_bound(struct rts_taskset* ts, int pluginid, uint32_t* min, uint32_t* max) {
    struct rts_task* t;
    iterator_t iterator;
    
    if(rts_taskset_is_empty(ts)) {
        *min = 1;
        *max = 99;
        return;
    }
    
    *min = UINT32_MAX;
    *max = 0;
    
    iterator = rts_taskset_iterator_init(ts);
    
    for(; iterator != NULL; iterator = iterator_get_next(iterator)) {
        t = rts_taskset_iterator_get_elem(iterator);
        
        if(t->pluginid == pluginid) {
            *min = MIN(*min, t->priority);
            *max = MAX(*max, t->priority);
        }   
    }
}

uint32_t prio_remap(uint32_t max_prio_s, uint32_t min_prio_s, uint32_t max_prio_u, uint32_t min_prio_u, uint32_t prio) {
    float slope;
    
    if((max_prio_u - min_prio_u) == 0)
        return min_prio_s;
    else
        slope = (max_prio_s - min_prio_s) / (float)(max_prio_u - min_prio_u);
    
    return min_prio_s + slope * (prio - min_prio_u);
}

void ts_recalc_utils(struct rts_plugin* this, struct rts_taskset* ts) {
    iterator_t iterator;
    struct rts_task* t;
    
    memset(this->util_used_percpu, 0, this->cpunum * sizeof(float));
    iterator = rts_taskset_iterator_init(ts);
    
    for(; iterator != NULL; iterator = iterator_get_next(iterator)) {
        t = rts_taskset_iterator_get_elem(iterator);
        
        if(t->pluginid == this->pluginid)
            this->util_used_percpu[t->cpu] += rts_task_utilization(t);
    }
}

void ts_recalc_prio(struct rts_plugin* this, struct rts_taskset* ts) {
    iterator_t iterator;
    struct rts_task* t;
    
    uint32_t max_prio_user;
    uint32_t min_prio_user;
    
    get_prio_bound(ts, this->pluginid, &min_prio_user, &max_prio_user);
    
    for(; iterator != NULL; iterator = iterator_get_next(iterator)) {
        t = rts_taskset_iterator_get_elem(iterator);
        
        if(t->pluginid == this->pluginid)
            t->schedprio = prio_remap(this->prio_max, this->prio_min, max_prio_user, min_prio_user, t->priority);
        
    }
}

int t_schedule(struct rts_task* t) {
    struct sched_param attr;
    cpu_set_t my_set;

    CPU_ZERO(&my_set);
    CPU_SET(t->cpu, &my_set);

    if(sched_setaffinity(t->tid, sizeof(cpu_set_t), &my_set) < 0)
        return -1;
    
    attr.sched_priority = t->schedprio;
    
    if(sched_setscheduler(t->tid, SCHED_RR, &attr) < 0)
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
    this->util_used_percpu[t->cpu] += rts_task_utilization(t);
}

void t_remove_from_utils(struct rts_plugin* this, struct rts_task* t) {
    this->util_used_percpu[t->cpu] -= rts_task_utilization(t);
}

void t_calc_prio(struct rts_plugin* this, struct rts_taskset* ts, struct rts_task* t) {
    uint32_t max_prio_user;
    uint32_t min_prio_user;
    
    get_prio_bound(ts, this->pluginid, &min_prio_user, &max_prio_user);
    t->schedprio = prio_remap(this->prio_max, this->prio_min, max_prio_user, min_prio_user, t->priority);
}

float t_test(struct rts_plugin* this, struct rts_taskset* ts, struct rts_task* t, float* free_utils) {
    float task_util;
    
    task_util = rts_task_utilization(t);
    
    for(int i = 0; i < this->cpunum; i++) {
        if(free_utils[i] >= task_util) {
            t->cpu = i;
            return 1;
        }
    }

    return 0;
}