#include "rts_lib.h"
#include "../daemon/lib/rts_utils.h"
#include <unistd.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>

#define MAX_EST_PERIOD 10000
#define MAX_EST_WCET 5000
#define MAX_ACT_NUMBER 500

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

int rts_refresh_sys(struct rts_access* c) {
    c->req.req_type = RTS_REFRESH_SYS;

    if(rts_access_send(c) < 0)
        return RTS_ERROR;
    if(rts_access_recv(c) < 0)
        return RTS_ERROR;

    if(c->rep.rep_type == RTS_REFRESH_SYS_ERR)
        return RTS_ERROR;
    
    return RTS_OK; 
}

int rts_refresh_single(struct rts_access* c, rsv_t rsvid) {
    c->req.req_type = RTS_REFRESH_SINGLE;
    c->req.payload.ids.rsvid = rsvid;

    if(rts_access_send(c) < 0)
        return RTS_ERROR;
    if(rts_access_recv(c) < 0)
        return RTS_ERROR;

    if(c->rep.rep_type == RTS_REFRESH_SINGLE_ERR)
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
    
    shatomic_put_value(&(tp->estimatedp), EST_NUM_ACTIVATION, 0);
    shatomic_put_value(&(tp->estimatedp), EST_PERIOD, MAX_EST_PERIOD);
    shatomic_put_value(&(tp->estimatedp), EST_WCET, MAX_EST_WCET);
    
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

uint64_t rts_params_get_est_param(struct rts_params* tp, int FLAG) {
    return shatomic_get_value(&(tp->estimatedp), FLAG);
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
    uint32_t t_perthread_act;
    
    t_act_num = shatomic_get_value(&(tp->estimatedp), EST_NUM_ACTIVATION);
    
    t_abs_act_prec = shatomic_get_value(&(tp->estimatedp), EST_ABS_ACTIVATION);
    t_abs_act_curr = get_time_now_ms(tp->clk);
    
    if(t_act_num > 0) {
        t_period_prec = shatomic_get_value(&(tp->estimatedp), EST_PERIOD);
        t_period = t_abs_act_curr - t_abs_act_prec;
        t_period_curr = REACTIVITY * t_period + (1 - REACTIVITY) * t_period_prec;
        shatomic_put_value(&(tp->estimatedp), EST_PERIOD, t_period_curr);
    }
    
    t_perthread_act = get_thread_time_ms(tp->clk);
    
    shatomic_put_value(&(tp->estimatedp), EST_PERTHREADCLK, t_perthread_act);
    shatomic_put_value(&(tp->estimatedp), EST_NUM_ACTIVATION, ++t_act_num);
    shatomic_put_value(&(tp->estimatedp), EST_ABS_ACTIVATION, t_abs_act_curr);
}

void rts_rsv_end(struct rts_params* tp) {
    uint32_t t_act_num;
    uint32_t t_wcet_curr;
    uint32_t t_wcet_prec;
    uint32_t t_perthread_prec;
    uint32_t t_perthread_curr;
    
    t_act_num = shatomic_get_value(&(tp->estimatedp), EST_NUM_ACTIVATION);
    t_wcet_prec = shatomic_get_value(&(tp->estimatedp), EST_WCET);
    t_perthread_prec = shatomic_get_value(&(tp->estimatedp), EST_PERTHREADCLK);  
    t_perthread_curr = get_thread_time_ms(tp->clk);
    
    t_wcet_curr = t_perthread_curr - t_perthread_prec;
    
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

// TASK TIME MANAG

void rts_thread_init(struct rts_thread* t, struct rts_params* p) {
    rts_thread_calc_exec(t, p->budget, p->period, p->deadline);
    rts_thread_calc_period(t, p->budget, p->period, p->deadline);
    get_time_now2(p->clk, &(t->t_activation_time));
}

void rts_thread_compute(struct rts_thread* t) {
    compute_for(t->t_wcet);
}

void rts_thread_wait_activation(struct rts_thread* t) {
    wait_next_activation(&(t->t_activation_time), t->t_period);
}

void rts_thread_calc_exec(struct rts_thread* t, int cfg_budg, int cfg_per, int cfg_dead) {
    if(cfg_budg != 0)
        t->t_wcet = cfg_budg;
    else if(cfg_dead != 0)
        t->t_wcet = cfg_dead;
    else if(cfg_per != 0)
        t->t_wcet = cfg_per;
    else
        t->t_wcet = rand() % MAX_EST_WCET;
}

void rts_thread_calc_period(struct rts_thread* t, int cfg_budg, int cfg_per, int cfg_dead) {
    if(cfg_per != 0)
        t->t_period = cfg_per;
    else if(cfg_dead != 0)
        t->t_period = DEADLINE_TO_PERIOD(cfg_dead);
    else if(cfg_budg != 0)
        t->t_period = WCET_TO_PERIOD(cfg_budg);
    else
        t->t_period = rand() % MAX_EST_PERIOD; 
}

void rts_thread_rand_activation_num(struct rts_thread* t) {
    t->t_activation_num_curr = 0;
    t->t_activation_num_tot = rand() % MAX_ACT_NUMBER;
}

void rts_thread_set_activation_num(struct rts_thread* t, int curr, int tot) {
    t->t_activation_num_curr = curr;
    t->t_activation_num_tot = tot;
}

int rts_thread_computation_ended(struct rts_thread* t) {    
    if(t->t_activation_num_curr < t->t_activation_num_tot)
        t->t_activation_num_curr++;
    else
        return 1;
    
    return 0;
}

void rts_thread_print_info(struct rts_thread* t, struct rts_params* p, int tid) {
    printf("THREAD %d INFO - WC: %d | PR: %d | ACT NUM: %d | ACT NUM TOT: %d | EST WC: %ld | EST PR: %ld | EST PER THR CLK: %ld\n",
            tid, 
            t->t_wcet, 
            t->t_period, 
            t->t_activation_num_curr,
            t->t_activation_num_tot,
            rts_params_get_est_param(p, EST_WCET),
            rts_params_get_est_param(p, EST_PERIOD),
            rts_params_get_est_param(p, EST_PERTHREADCLK));
}

float rts_thread_calc_budget(struct rts_thread* t, struct rts_params* p) {
    float period;
    float wcet;
    
    if(t->t_period != 0)
        period = t->t_period;
    else
        period = rts_params_get_est_param(p, EST_PERIOD);
    
    if(t->t_wcet != 0)
        wcet = t->t_wcet;
    else
        wcet = rts_params_get_est_param(p, EST_WCET);
    
    return wcet / period;
}

float rts_thread_calc_rem_budget(struct rts_thread* t, struct rts_params* p) {    
    uint32_t wcet;
    uint32_t period;
    uint32_t cputimenow;
    uint32_t cputimeact;
    float budget_total;
    float budget_used;
   
    if(t->t_wcet != 0)
        wcet = t->t_wcet;
    else
        wcet = rts_params_get_est_param(p, EST_WCET);
    
    if(t->t_period != 0)
        period = t->t_period;
    else
        period = rts_params_get_est_param(p, EST_PERIOD);
    
    cputimenow = get_thread_time_ms();
    cputimeact = rts_params_get_est_param(p, EST_PERTHREADCLK);        
            
    budget_total = rts_thread_calc_budget(t, p);
    budget_used = (wcet - (cputimenow - cputimeact)) / period;
    
    return budget_total - budget_used;
}