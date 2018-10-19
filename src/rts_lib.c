#include "rts_lib.h"

int rts_daemon_connect(struct rts_channel* cc) {
    if(rts_channel_init(cc) < 0)
        return RTS_SOCK_ISSUE;
        
    if(rts_channel_connect(cc) < 0)
        return RTS_DAEMON_UNREACHBLE;

    return RTS_CONNECTED;
}

float rts_cap_query(struct rts_channel* c, enum QUERY_TYPE type) {
    struct rts_request req;
    struct rts_reply rep;

    req.req_type = RTS_CAP_QUERY;
    req.payload.query_type = type;
    rts_channel_send(c, &req);
    rts_channel_recv(c, &rep);

    if(rep.rep_type == RTS_CAP_QUERY_INVALID)
        return RTS_ERROR;
    
    return rep.payload; 
}

void rts_params_init(struct rts_params *tp) {
    memset(tp, 0, sizeof(struct rts_params));
}

void rts_set_period(struct rts_params* tp, uint32_t period) {
    tp->period = period;
}

void rts_set_budget(struct rts_params* tp, uint32_t budget) {
    tp->budget = budget;
}

void rts_set_deadline(struct rts_params* tp, uint32_t deadline) {
    tp->deadline = deadline;
}

void rts_set_priority(struct rts_params* tp, uint32_t priority) {
    tp->priority = priority;
}

int rts_create_rsv(struct rts_channel* c, struct rts_params* tp, rsv_t* id) {
    struct rts_request req;
    struct rts_reply rep;

    req.req_type = RTS_RSV_CREATE;
    memcpy(&(req.payload.param), tp, sizeof(struct rts_params));
    rts_channel_send(c, &req);
    rts_channel_recv(c, &rep);

    if(rep.rep_type == RTS_RSV_NOT_CREATED)
        return RTS_NOT_GUARANTEED;

    *id = rep.payload;
    return RTS_GUARANTEED;
}

int rts_rsv_attach_thread(struct rts_channel* c, rsv_t id, pid_t pid) {
    struct rts_request req;
    struct rts_reply rep;

    req.req_type = RTS_RSV_ATTACH;
    req.payload.ids.rsvid = id;
    req.payload.ids.pid = pid;
    rts_channel_send(c, &req);
    rts_channel_recv(c, &rep);

    if(rep.rep_type == RTS_RSV_ATTACH_INVALID)
        return RTS_ERROR;

    return RTS_OK;
}

int rts_rsv_detach_thread(struct rts_channel* c, rsv_t id) {
    struct rts_request req;
    struct rts_reply rep;

    req.req_type = RTS_RSV_DETACH;
    req.payload.ids.rsvid = id;
    rts_channel_send(c, &req);
    rts_channel_recv(c, &rep);

    if(rep.rep_type == RTS_RSV_DETACH_INVALID)
        return RTS_ERROR;

    return RTS_OK;
}