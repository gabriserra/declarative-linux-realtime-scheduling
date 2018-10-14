
#define _GNU_SOURCE
#include "lib/rts_channel.h"
#include <stdint.h>
#include <time.h>
#include <fcntl.h>
#include <unistd.h>

#define RTS_CAP_BUDGET 1
#define RTS_CAP_REMAINING_BUDGET 2
#define RTS_GUARANTEED 3

typedef uint32_t rsv_id;

struct rts_params {
    clockid_t 		clk;
    uint32_t		budget;		// worst case ex time [microseconds]
	uint32_t 		period;		// period of task [millisecond]
	uint32_t 		deadline;	// relative deadline [millisecond]
	uint32_t 		priority;	// priority of task [LOW_PRIO, HIGH_PRIO]
};

float rts_cap_query(int msg_type) {
    struct rts_msg_req req;
    struct rts_msg_rep rep;

    rts_req_set_message(&req, msg_type);
    rts_send_message(&req, &rep);

    // fare qualcosa con la risposta
    
    return rep.response; 
}

void rts_params_init(struct rt_task *tp) {
    memset(tp, 0, sizeof(struct rts_params));
}

void rts_set_period(struct rt_task *tp, uint32_t period) {
    tp->period = period;
}

uint32_t rts_get_period(struct rt_task *tp) {
    return tp->period;
}

void rts_set_budget(struct rt_task *tp, uint32_t budget) {
    tp->budget = budget;
}

uint32_t rts_get_budget(struct rt_task *tp) {
    return tp->budget;
}

void rts_set_deadline(struct rt_task *tp, uint32_t deadline) {
    tp->deadline = deadline;
}

uint32_t rts_get_deadline(struct rt_task *tp) {
    return tp->deadline;
}

void rts_set_priority(struct rt_task *tp, uint32_t priority) {
    tp->priority = priority;
}

uint32_t rts_get_priority(struct rt_task *tp) {
    return tp->priority;
}

int rts_create_rsv(struct rt_task *tp, rsv_id* id) {
    //
}
