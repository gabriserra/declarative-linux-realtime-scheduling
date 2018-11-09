/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

#include <sched.h>
#include <stdlib.h>

struct rts_sched_plugin {
    enum SCHED name;
    int prio_min;
    int prio_max;
    float used_budget;
    float (*test)(struct rts_task* t, float free_budget);
    void (*schedule)(struct rts_sched_plugin* this, struct rts_task* t);
    void (*deschedule)(struct rts_sched_plugin* this, struct rts_task* t);
    void (*calc_prio) (struct rts_sched_plugin* this, struct rts_taskset* ts, struct rts_task* t);
    void (*calc_budget)(struct rts_sched_plugin* this, struct rts_taskset* ts);
};

float test(struct rts_task* t, float free_budget) {
    if(free_budget < rts_task_utilization(t))
        return 0;
        
    return 1;
}

void calc_prio(struct rts_sched_plugin* this, struct rts_taskset* ts, struct rts_task* t) {
    if(t->priority > this->prio_max)
        t->priority = this->prio_max;
    else if(t->priority < this->prio_min)
        t->priority = this->prio_min;
    
    return;
}

void schedule(struct rts_task* t) {
    struct sched_param attr;
    
    attr.sched_priority = t->priority;
    sched_setscheduler(t->tid, SCHED_RR, &attr);
}

void deschedule(struct rts_task* t) {
    struct sched_param attr;
    
    attr.sched_priority = t->priority;
    sched_setscheduler(t->tid, SCHED_RR, &attr);
}

void calc_budget(struct rts_sched_plugin* this, struct rts_taskset* ts) {
    node_ptr* iterator;
    struct rts_task* t;
    
    for(iterator = ts->tasks->root; iterator != NULL; iterator = iterator->next) {
        t = (struct rts_task*)iterator->elem;
        
        if(t->sched == this->name)
            this->used_budget += rts_task_utilization(t);
        
    }
}
