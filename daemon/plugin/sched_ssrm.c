#include "../lib/rts_taskset.h"
#include <sched.h>
#include <stdlib.h>
#include <float.h>
#include <stdint.h>

void calc_prio(struct rts_plugin* this, struct rts_taskset* ts, struct rts_task* t) {
    struct node_ptr* n;
    struct rts_task* t;
    unsigned int last_priority;

    rts_taskset_sort(ts, PERIOD, ASC);
    n = rts_taskset_get_i_node(ts, 0);
    t = (struct rts_task*) n->elem;
    last_priority = 1;

    while (n != NULL) {
        t->priority = last_priority;
        n = rts_taskset_get_next_node(ts, n);
        t = (struct rts_task*) n->elem;
        last_priority++;
    }
}

float test(struct rts_taskset* ts, struct rts_task* t, float free_budget) {
    struct node_ptr* iter;
    struct rts_task* t_ssrm;
    struct rts_taskset ts_ssrm;
    
    if(free_budget < rts_task_utilization(t))
        return 0;
    
    rts_taskset_init(&ts_ssrm);
    
    for(iter = ts_ssrm->tasks->root; iter != NULL; iter = iter->next) {
        t_ssrm = (struct rts_task*) iter->elem;
        if(t_ssrm->plugin == this->name)
            rts_taskset_add_sorted_pr(&ts_ssrm, t_ssrm);
    }
    
    if(hyperbolic_bound(&ts_ssrm))
        return 0;
    
    if(workload_analysis(&ts_ssrm))
        return 0;
        
    // check periodo reale e wcet reale
    int required = 2; // wcet and period
    int got = 0;
    
    if(t->wcet != 0)
        got++;
    if(t->period != 0)
        got++;
    
    return (got/ (float)required);
}

//----------------------------------------------------------
// WORKLOAD ANALYSIS: perform the sched. analysis under fp
//----------------------------------------------------------

unsigned int hyperbolic_bound(struct rts_taskset* ts) {
	float res;
	unsigned int i;
	struct node_ptr* n;
	struct rts_task* t;

	n = rts_taskset_get_i_node(ts, 0);
	t = (struct rts_task*) n->elem;
	res = (t->wcet / t->period) + 1;

	for(i = 1; i < rts_taskset_get_size(ts); i++) {
		n = rts_taskset_get_next_node(ts, n);
		t = (struct rts_task*) n->elem;
		res *= (t->wcet / t->period) + 1;
	}

	if(res > 2)
		return 0;
	
	return 1;
}

// da riscrivere con accesso lineare!!!!!!!

unsigned int workload_analysis(struct rts_taskset* ts) {
	struct list_int points;
	int n_points, j, jth_point;
	unsigned int i, is_schedulable;

	list_int_init(&points);

	for(i = 0; i < rts_taskset_get_size(ts); i++) {
		
		is_schedulable = 0;
		n_points = testing_set_fp(ts, &points, i);

		if(n_points > 0) {
			
			for (j = 0; j < n_points; j++) {
				jth_point = list_int_get_i_elem(&points, j);
				if (workload_function(ts, i, jth_point) <= jth_point)
					is_schedulable = 1;
			}

			if(!is_schedulable)
				break;
		}
	}

	return is_schedulable;
}

//------------------------------------------------------------------------------
// WORKLOAD FUNTION: compute the workload function at time t for the given task
//------------------------------------------------------------------------------

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
	unsigned int j, k, index, already_present, n_points = 0;

	if (i >= rts_taskset_get_size(ts))
		return -1;

	ti_deadline = rts_taskset_get_i_task(ts, i)->deadline;
	list_add_top(points, (void*) &ti_deadline);
	
	for (j = 0; j < i; j++) {
		k = 1;
		tj_period = rts_taskset_get_i_task(ts, j)->period;
		
		while (k * tj_period < ti_deadline) {
			if(list_search_elem(points, k * tj_period) == NULL)
				list_add_sorted(points, k * tj_period);
			k++;
		}
	}

	return list_get_size(points);
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

void calc_budget(struct rts_plugin* this, struct rts_taskset* ts) {
    node_ptr* iterator;
    struct rts_task* t;
    
    for(iterator = ts->tasks->root; iterator != NULL; iterator = iterator->next) {
        t = (struct rts_task*)iterator->elem;
        
        if(t->plugin == this->type)
            this->used_budget += rts_task_utilization(t);
        
    }
}

