#define _GNU_SOURCE

#include "../lib/rts_taskset.h"
#include "../components/list_int.h"
#include <sched.h>
#include <stdlib.h>
#include <float.h>
#include <stdint.h>
#include <math.h>
#include <string.h>
#include <sys/sysinfo.h>

//----------------------------------------------------------
// WORKLOAD ANALYSIS: perform the sched. analysis under fp
//----------------------------------------------------------

int cmp(int elem1, int elem2) {
    if(elem1 > elem2)
        return 1;
    else if(elem1 < elem2)
        return -1;
    
    return 0;
}

unsigned int workload_function(struct rts_taskset* ts, unsigned int i, unsigned int t) {
    struct rts_task* tj;
    unsigned int j, res;

    res = rts_taskset_get_i_task(ts, i)->wcet;

    for (j = 0; j < i; j++) {
        tj = rts_taskset_get_i_task(ts, j);
        res += (unsigned int) ceil((double) t / tj->period) * tj->wcet;
    }


    return res;
}

// Calculate testing set points BB04
int testing_set_fp(struct rts_taskset* ts, struct list_int* points, unsigned int i) {
    unsigned int tj_period, ti_deadline;
    unsigned int j, k = 0;

    if (i >= rts_taskset_get_size(ts))
            return -1;

    ti_deadline = rts_taskset_get_i_task(ts, i)->deadline;
    list_int_add_top(points, ti_deadline);

    for (j = 0; j < i; j++) {
        k = 1;
        tj_period = rts_taskset_get_i_task(ts, j)->period;

        while (k * tj_period < ti_deadline) {
            if(list_int_search_elem(points, k * tj_period) == NULL)
                list_int_add_sorted(points, k * tj_period, cmp);
            k++;
        }
    }

    return list_int_get_size(points);
}

// da riscrivere con accesso lineare!!!!!!!

unsigned int workload_analysis(struct rts_taskset* ts) {
    struct list_int points;
    int n_points, j, jth_point;
    unsigned int i, is_schedulable = 0;

    list_int_init(&points);

    for(i = 0; i < rts_taskset_get_size(ts); i++) {

        is_schedulable = 0;
        n_points = testing_set_fp(ts, &points, i);

        if(n_points > 0) {

            for (j = 0; j < n_points; j++) {
                jth_point = *(list_int_get_i_elem(&points, j));
                if (workload_function(ts, i, jth_point) <= jth_point)
                    is_schedulable = 1;
            }

            if(!is_schedulable)
                    break;
        }
    }

    return is_schedulable;
}

unsigned int hyperbolic_bound(struct rts_taskset* ts) {
    float res;
    iterator_t iterator;
    struct rts_task* t;

    res = 1;
    iterator = rts_taskset_iterator_init(ts);
    
    for(; iterator != NULL; iterator = iterator_get_next(iterator)) {
        t = rts_taskset_iterator_get_elem(iterator);
        res *= (t->wcet / t->period) + 1;
    }

    if(res > 2)
        return 0;

    return 1;
}

void sort_taskset(struct rts_plugin* this, struct rts_taskset* ts, struct rts_taskset* ts_ssrm, int cpu) {
    struct rts_task* t;
    iterator_t iterator;
    
    iterator = rts_taskset_iterator_init(ts);
    
    for(; iterator != NULL; iterator = iterator_get_next(iterator)) {
        t = rts_taskset_iterator_get_elem(iterator);
        if(t->pluginid == this->pluginid && t->cpu == cpu)
            rts_taskset_add_sorted_pr(ts_ssrm, t);
    }
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
    struct rts_task* t_ssrm;
    struct rts_taskset ts_ssrm;
    unsigned int priority;
    
    for(int i = 0; i < this->cpunum; i++) {
        rts_taskset_init(&ts_ssrm);
        sort_taskset(this, ts, &ts_ssrm, i);
    
        iterator = rts_taskset_iterator_init(&ts_ssrm);
        priority = this->prio_max;

        for (; iterator != NULL; iterator = iterator_get_next(iterator)) {
            t_ssrm = rts_taskset_iterator_get_elem(iterator);
            t_ssrm->schedprio = priority;
            priority--;
        }
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
    
    if(sched_setscheduler(t->tid, SCHED_FIFO, &attr) < 0)
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

void t_assign_cpu(struct rts_plugin* this, struct rts_task* t, float* free_utils) {
    float task_util;
    
    task_util = rts_task_utilization(t);
    
    for(int i = 0; i < this->cpunum; i++)
        if(free_utils[i] >= task_util)
            t->cpu = i;
}

void t_add_to_utils(struct rts_plugin* this, struct rts_task* t) {
    this->util_used_percpu[t->cpu] += rts_task_utilization(t);
}

void t_remove_from_utils(struct rts_plugin* this, struct rts_task* t) {
    this->util_used_percpu[t->cpu] -= rts_task_utilization(t);
}

void t_calc_prio(struct rts_plugin* this, struct rts_taskset* ts, struct rts_task* t) {
    iterator_t iterator;
    struct rts_task* t_ssrm;
    struct rts_taskset ts_ssrm;
    unsigned int priority;
        
    rts_taskset_init(&ts_ssrm);
    sort_taskset(this, ts, &ts_ssrm, t->cpu);
    
    iterator = rts_taskset_iterator_init(&ts_ssrm);
    priority = this->prio_max;

    for (; iterator != NULL; iterator = iterator_get_next(iterator)) {
        t_ssrm = rts_taskset_iterator_get_elem(iterator);
        
        if(t_ssrm->id == t->id) {
            t->schedprio = priority;
            break;
        }
        
        priority--;
    }
}

float t_test(struct rts_plugin* this, struct rts_taskset* ts, struct rts_task* t, float* free_utils) {
    int got;
    int required;
    int free_cpu = -1;
    float task_util;
    struct rts_taskset ts_ssrm;
    
    task_util = rts_task_utilization(t);
       
    for(int i = 0; i < this->cpunum && free_cpu == -1; i++) {
        if(free_utils[i] < task_util)
            continue;
        
        rts_taskset_init(&ts_ssrm);
        sort_taskset(this, ts, &ts_ssrm, i);

        if(rts_taskset_get_size(&ts_ssrm) > (this->prio_max - this->prio_min + 1))
            continue;

        if(hyperbolic_bound(&ts_ssrm))
            continue;

        if(workload_analysis(&ts_ssrm))
            continue;
        
        free_cpu = i;
    }
    
    if(free_cpu == -1)
        return 0;
    
    t->cpu = free_cpu;
    
    required = 2;
    got = 0;
    
    if(t->wcet != 0)
        got++;
    if(t->period != 0)
        got++;
    
    return (got/ (float)required);
}

