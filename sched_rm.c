
#include "sched_rm.h"
#include "rt_taskset.h"
#include "rt_task.h"
#include "list.h"

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
*/

//----------------------------------------------------------
// WORKLOAD ANALYSIS: perform the sched. analysis under fp
//----------------------------------------------------------

unsigned int workload_analysis(struct rt_taskset* ts) {
	int n_testing_set, j;
	struct list points;
	unsigned int i, is_schedulable;

	list_init(&points);

	for(i = 0; i < taskset_get_size(ts); i++) {
		is_schedulable = 0;
		n		
		if((n_testing_set = testing_set_fp(ts, testing_set, i)) > 0) {
			for (j = 0; j < n_testing_set; j++)
				if (workload_function(ts, i, testing_set[j]) <= testing_set[j])
					is_schedulable = 1;	
			if(!is_schedulable)
				break;
		}
	}

	return is_schedulable;
}

// Calculate testing set points BB04
int testing_set_fp(struct rt_taskset* ts, struct list* points, unsigned int i) {
	unsigned int tj_period, ti_deadline;
	unsigned int j, k, index, already_present, n_points = 0;

	if (i >= taskset_get_size(ts))
		return -1;

	ti_deadline = taskset_get_i_task(ts, i)->deadline;
	list_add_top(points, (void*) &ti_deadline)
	points[n_points++] = ti_deadline;
	
	for (j = 0; j < i; j++) {
		k = 1;
		tj_period = taskset_get_i_task(ts, j)->period;
		
		while (k * tj_period < ti_deadline) {
			
			already_present = 0;
			for(index = 0; index < n_points; index++)
				if(points[index] == k * ts->tasks[j].T)
					already_present = 1;
				
			if(!already_present)	
				points[n_points++] =  k * ts->tasks[j].T;
			
			k++;
		}
	}

	bubble_sort(points, n_points);

	return n_points;
}