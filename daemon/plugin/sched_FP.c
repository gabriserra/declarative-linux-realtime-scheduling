#include "../lib/rts_taskset.h"
#include <sched.h>
#include <stdlib.h>
#include <float.h>
#include <stdint.h>

#define MIN(val1, val2) (val1 < val2 ? val1 : val2)
#define MAX(val1, val2) (val1 > val2 ? val1 : val2)

static void get_prio_bound(struct rts_taskset* ts, enum plugin sched, uint32_t* min, uint32_t* max) {
    struct rts_task* t;
    iterator_t iterator;
    
    *min = UINT32_MAX;
    *max = 0;
    
    iterator = rts_taskset_iterator_init(ts);
    
    for(; iterator != NULL; iterator = iterator_get_next(iterator)) {
        t = rts_taskset_iterator_get_elem(iterator);
        
        if(t->plugin == sched) {
            *min = MIN(*min, t->priority);
            *max = MAX(*max, t->priority);
        }   
    }
}

float test(struct rts_plugin* this, struct rts_taskset* ts, struct rts_task* t, float free_budget) {    
    if(free_budget < rts_task_utilization(t))
        return 0;
    
    if(t->priority == 0)
        return 0;
        
    return 1;
}

void schedule(struct rts_task* t) {
    struct sched_param attr;
    
    attr.sched_priority = t->schedprio;
    sched_setscheduler(t->tid, SCHED_FIFO, &attr);
}

void deschedule(struct rts_task* t) {
    struct sched_param attr;
    
    attr.sched_priority = 0;
    sched_setscheduler(t->tid, SCHED_OTHER, &attr);
}

void calc_prio(struct rts_plugin* this, struct rts_taskset* ts, struct rts_task* t) {
    float slope;
    uint32_t max_prio_user;
    uint32_t min_prio_user;
    
    get_prio_bound(ts, this->type, &min_prio_user, &max_prio_user);
    
    if((max_prio_user - min_prio_user) == 0)
        slope = FLT_MAX;
    else
        slope = (this->prio_max - this->prio_min) / (float)(max_prio_user - min_prio_user);
    
    t->schedprio = this->prio_min + slope * (t->priority - min_prio_user);
}

void calc_budget(struct rts_plugin* this, struct rts_taskset* ts) {
    iterator_t iterator;
    struct rts_task* t;
    
    iterator = rts_taskset_iterator_init(ts);
    
    for(; iterator != NULL; iterator = iterator_get_next(iterator)) {
        t = rts_taskset_iterator_get_elem(iterator);
        
        if(t->plugin == this->type)
            this->used_budget += rts_task_utilization(t);
        
    }
}