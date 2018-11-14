#include "rts_lib.h"
#include "../daemon/lib/rts_utils.h"
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

int rts_params_init(struct rts_params *tp) {
    memset(tp, 0, sizeof(struct rts_params));
    shatomic_init(&(tp->estimatedp));
    
    if(shatomic_create(&(tp->estimatedp), EST_NVALUE) < 0)
        return -1;
    
    if(shatomic_attach(&(tp->estimatedp)) < 0)
        return -1;
    
    return 0;
}

void rts_set_clock(struct rts_params* tp, clockid_t clk) {
    tp->clk = clk;
}

clockid_t rts_get_clock(struct rts_params* tp) {
    return tp->clk;
}

void rts_set_period(struct rts_params* tp, uint32_t period) {
    tp->period = period;
}

int rts_get_period(struct rts_params* tp) {
    return tp->period;
}

void rts_set_budget(struct rts_params* tp, uint32_t budget) {
    tp->budget = budget;
}

int rts_get_budget(struct rts_params* tp) {
    return tp->budget;
}

void rts_set_deadline(struct rts_params* tp, uint32_t deadline) {
    tp->deadline = deadline;
}

int rts_get_deadline(struct rts_params* tp) {
    return tp->deadline;
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

void rts_rsv_begin(struct rts_params* tp) {
    uint32_t t_act_num;
    uint32_t t_period;
    uint32_t t_period_curr;
    uint32_t t_period_prec;
    uint32_t t_abs_act_prec;
    uint32_t t_abs_act_curr;
    
    t_act_num = shatomic_get_value(&(tp->estimatedp), EST_NUM_ACTIVATION);
    t_abs_act_prec = shatomic_get_value(&(tp->estimatedp), EST_ABS_ACTIVATION);
    t_period_prec = shatomic_get_value(&(tp->estimatedp), EST_PERIOD);
   
    t_abs_act_curr = get_time_now_ms(tp->clk);
    t_period = t_abs_act_curr - t_abs_act_prec;
    
    t_period_curr = REACTIVITY * t_period + (1 - REACTIVITY) * t_period_prec;
    
    shatomic_put_value(&(tp->estimatedp), EST_NUM_ACTIVATION, ++t_act_num);
    shatomic_put_value(&(tp->estimatedp), EST_ABS_ACTIVATION, t_abs_act_curr);
    shatomic_put_value(&(tp->estimatedp), EST_PERIOD, t_period_curr);
}

void rts_rsv_end(struct rts_params* tp) {
    uint32_t t_abs_act;
    uint32_t t_abs_fin;
    uint32_t t_act_num;
    uint32_t t_wcet_curr;
    uint32_t t_wcet_prec;
    
    t_act_num = shatomic_get_value(&(tp->estimatedp), EST_NUM_ACTIVATION);
    t_abs_act = shatomic_get_value(&(tp->estimatedp), EST_ABS_ACTIVATION);  
    t_wcet_prec = shatomic_get_value(&(tp->estimatedp), EST_WCET);
    t_abs_fin = get_time_now_ms(tp->clk);
    
    t_wcet_curr = t_abs_fin - t_abs_act;
    
    if(t_act_num > 1)
        t_wcet_curr = ((t_wcet_prec > t_wcet_curr) ? t_wcet_prec : t_wcet_curr);
        
    shatomic_put_value(&(tp->estimatedp), EST_WCET, t_wcet_curr); 
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

int rts_daemon_deconnect(struct rts_access* c) {
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