#include "rts_lib.h"
#include <unistd.h>
#include <string.h>

int rts_daemon_connect(struct rts_access* c) {
    if(rts_access_init(c) < 0)
        return RTS_ERROR;
        
    if(rts_access_connect(c) < 0)
        return RTS_ERROR;

    c->req.req_type = RTS_CONNECTION;
    c->req.payload.ids.pid = getpid();

    if(rts_access_send(c) < 0)
        return RTS_ERROR;
    if(rts_access_recv(c) < 0)
        return RTS_ERROR;

    if(c->rep.rep_type == RTS_CONNECTION_ERR)
        return RTS_ERROR;

    return RTS_OK;
}

float rts_cap_query(struct rts_access* c, enum QUERY_TYPE type) {
    c->req.req_type = RTS_CAP_QUERY;
    c->req.payload.query_type = type;

    if(rts_access_send(c) < 0)
        return RTS_ERROR;
    if(rts_access_recv(c) < 0)
        return RTS_ERROR;

    if(c->rep.rep_type == RTS_CAP_QUERY_ERR)
        return RTS_ERROR;
    
    return c->rep.payload; 
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

void rts_params_cleanup(struct rts_params* tp) {
    rts_params_init(tp);
}

int rts_create_rsv(struct rts_access* c, struct rts_params* tp, rsv_t* id) {
    c->req.req_type = RTS_RSV_CREATE;
    memcpy(&(c->req.payload.param), tp, sizeof(struct rts_params));
    
    if(rts_access_send(c) < 0)
        return RTS_ERROR;
    if(rts_access_recv(c) < 0)
        return RTS_ERROR;

    if(c->rep.rep_type == RTS_RSV_CREATE_UN || c->rep.rep_type == RTS_RSV_CREATE_ERR)
        return RTS_NOT_GUARANTEED;

    *id = (rsv_t) c->rep.payload;
    return RTS_GUARANTEED;
}

int rts_rsv_attach_thread(struct rts_access* c, rsv_t id, pid_t pid) {
    c->req.req_type = RTS_RSV_ATTACH;
    c->req.payload.ids.rsvid = id;
    c->req.payload.ids.pid = pid;

    if(rts_access_send(c) < 0)
        return RTS_ERROR;
    if(rts_access_recv(c) < 0)
        return RTS_ERROR;

    if(c->rep.rep_type == RTS_RSV_ATTACH_ERR)
        return RTS_ERROR;

    return RTS_OK;
}

int rts_rsv_detach_thread(struct rts_access* c, rsv_t id) {
    c->req.req_type = RTS_RSV_DETACH;
    c->req.payload.ids.rsvid = id;

    if(rts_access_send(c) < 0)
        return RTS_ERROR;
    if(rts_access_recv(c) < 0)
        return RTS_ERROR;

    if(c->rep.rep_type == RTS_RSV_DETACH_ERR)
        return RTS_ERROR;

    return RTS_OK;
}

int rts_rsv_get_remaining_budget(struct rts_access* c, rsv_t id, float* budget) {
    c->req.req_type = RTS_RSV_QUERY;
    c->req.payload.ids.rsvid = id;    

    if(rts_access_send(c) < 0)
        return RTS_ERROR;
    if(rts_access_recv(c) < 0)
        return RTS_ERROR;

    if(c->rep.rep_type == RTS_RSV_QUERY_ERR)
        return RTS_ERROR;

    *budget = c->rep.payload;
    return RTS_OK;
}

int rts_rsv_destroy(struct rts_access* c, rsv_t id) {
    c->req.req_type = RTS_RSV_DESTROY;
    c->req.payload.ids.rsvid = id;

    if(rts_access_send(c) < 0)
        return RTS_ERROR;
    if(rts_access_recv(c) < 0)
        return RTS_ERROR;

    if(c->rep.rep_type == RTS_RSV_DESTROY_ERR)
        return RTS_ERROR;

    return RTS_OK;
}

int rts_rsv_deconnect(struct rts_access* c) {
    c->req.req_type = RTS_DECONNECTION;
    c->req.payload.ids.pid = getpid();

    if(rts_access_send(c) < 0)
        return RTS_ERROR;
    if(rts_access_recv(c) < 0)
        return RTS_ERROR;

    if(c->rep.rep_type == RTS_DECONNECTION_ERR)
        return RTS_ERROR;

    return RTS_OK;
}