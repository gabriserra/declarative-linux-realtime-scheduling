#include "lib/rts_channel.h"

void rts_daemon_connect(struct rts_channel_c* cc) {
    rts_channel_c_init(cc);
}

float rts_cap_query_budget(struct rts_channel_c* c) {
    struct rts_request req;
    struct rts_reply rep;

    req.req_type = RTS_CAP_QUERY;
    req.payload.query_type = RTS_BUDGET;
    rts_channel_c_send(c, &req);
    rts_channel_c_recv(c, &rep);

    if(rep.rep_type == RTS_CAP_QUERY_INVALID)
        return -1;
    
    return rep.payload; 
}

float rts_cap_query_remaining_budget(struct rts_channel_c* c) {

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
