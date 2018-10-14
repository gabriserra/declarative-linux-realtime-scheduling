
#include "sched_rm.h"
#include "../lib/rt_taskset.h"
#include "../lib/rt_task.h"
#include "../assets/list_int.h"

/* SCHED_RM
SE D = T
SCHEDULABILITA SUFF HB
EXACT RTA

SE D < T
EXACT RTA

for all i Ri <= Di
Ri = Ci + sum k = 1..i-1 ceil(Ri/Tk) * Ck
(e utilizzazione non deve superare il max)

priorita di ogni task è proporzionale al periodo P = 1/T
DM P = 1/D

Con * mi servono metodi nel padre
* calcola priorità
* testa se schedulabile (con tolleranza)
    - testa se schedulabile nec
    - testa se schedulabile suff
* schedula?
*/

void sched_rm_calc_priority(struct rt_taskset* ts) {
	struct node_ptr* n;
	struct rt_task* t;
	unsigned int last_priority;

	taskset_sort(ts, PERIOD, DSC);
	n = taskset_get_i_node(ts, 0);
	t = (struct rt_task*) n->elem;
	last_priority = 1;

	while (n != NULL) {
		t->priority = last_priority;
		n = taskset_get_next_node(ts, n);
		t = (struct rt_task*) n->elem;
		last_priority++;
	}
} 

//----------------------------------------------------------
// WORKLOAD ANALYSIS: perform the sched. analysis under fp
//----------------------------------------------------------

unsigned int hyperbolic_bound(struct rt_taskset* ts) {
	float res;
	unsigned int i;
	struct node_ptr* n;
	struct rt_task* t;

	n = taskset_get_i_node(ts, 0);
	t = (struct rt_task*) n->elem;
	res = (t->wcet / t->period) + 1;

	for(i = 1; i < taskset_get_size(ts); i++) {
		n = taskset_get_next_node(ts, n);
		t = (struct rt_task*) n->elem;
		res *= (t->wcet / t->period) + 1;
	}

	if(res > 2)
		return 0;
	
	return 1;
}

// da riscrivere con accesso lineare!!!!!!!

unsigned int workload_analysis(struct rt_taskset* ts) {
	struct list_int points;
	int n_points, j, jth_point;
	unsigned int i, is_schedulable;

	list_int_init(&points);

	for(i = 0; i < taskset_get_size(ts); i++) {
		
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

unsigned int workload_function(struct rt_taskset* ts, unsigned int i, unsigned int t) {
	struct rt_task* tj;
	unsigned int j, res;
	
	res = taskset_get_i_task(ts, i)->wcet;
	
	for (j = 0; j < i; j++) {
		tj = taskset_get_i_task(ts, j);
		res += (unsigned int) ceil((double) t / tj->period) * tj->wcet;
	}
		

	return res;
}

// Calculate testing set points BB04
int testing_set_fp(struct rt_taskset* ts, struct list_int* points, unsigned int i) {
	unsigned int tj_period, ti_deadline;
	unsigned int j, k, index, already_present, n_points = 0;

	if (i >= taskset_get_size(ts))
		return -1;

	ti_deadline = taskset_get_i_task(ts, i)->deadline;
	list_add_top(points, (void*) &ti_deadline);
	
	for (j = 0; j < i; j++) {
		k = 1;
		tj_period = taskset_get_i_task(ts, j)->period;
		
		while (k * tj_period < ti_deadline) {
			if(list_search_elem(points, k * tj_period) == NULL)
				list_add_sorted(points, k * tj_period);
			k++;
		}
	}

	return list_get_size(points);
}