/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

#include "rts_daemon.h"
#include "rts_utils.h"
#include <stdlib.h>
 #include <signal.h>


static struct rts_reply req_connection(struct rts_daemon* data, int cli_id, pid_t ppid) {
    struct rts_reply rep;
    
    rts_carrier_set_pid(&(data->chann), cli_id, ppid);
    rep.rep_type = RTS_CONNECTION_OK;
    
    return rep;
}

static struct rts_reply req_cap_query(struct rts_daemon* data, enum QUERY_TYPE type) {
    struct rts_reply rep;
    
    switch(type) {
        case RTS_BUDGET:
            rep.payload = read_rt_kernel_budget();
            rep.rep_type = RTS_CAP_QUERY_OK;
            break;
        case RTS_REMAINING_BUDGET:
            rep.payload = rts_scheduler_remaining_budget(&(data->sched));
            rep.rep_type = RTS_CAP_QUERY_OK;
            break;
        default:
            rep.rep_type = RTS_CAP_QUERY_ERR;
    }
    
    return rep;
}

static struct rts_reply req_rsv_create(struct rts_daemon* data, struct rts_params* p) {
    int rsv_id;
    struct rts_reply rep;
    
    rsv_id = rts_scheduler_rsv_create(&(data->sched), p);
            
    if(rsv_id < 0) {
        rep.rep_type = RTS_RSV_CREATE_ERR;
        rep.payload = -1;
    } else {
        rep.rep_type = RTS_RSV_CREATE_OK;
        rep.payload = rsv_id;
    }
    
    return rep;
}

static struct rts_reply req_rsv_attach(struct rts_daemon* data, rsv_t rsvid, pid_t pid) {
    struct rts_reply rep;
    
    if(rts_scheduler_rsv_attach(&(data->sched), rsvid, pid) < 0)
        rep.rep_type = RTS_RSV_ATTACH_ERR;
    else
        rep.rep_type = RTS_RSV_ATTACH_OK;
    
    return rep;
}

static struct rts_reply req_rsv_detach(struct rts_daemon* data, rsv_t rsvid) {
    struct rts_reply rep;
    
    if(rts_scheduler_rsv_detach(&(data->sched), rsvid) < 0)
        rep.rep_type = RTS_RSV_DETACH_ERR;
    else
        rep.rep_type = RTS_RSV_DETACH_OK;
    
    return rep;   
}

static struct rts_reply req_rsv_query(struct rts_daemon* data, rsv_t rsvid, enum QUERY_TYPE type) {
    struct rts_reply rep;
    
    switch(type) {
        case RTS_BUDGET:
            rep.rep_type = RTS_RSV_QUERY_OK;
            rep.payload = rts_scheduler_rsv_budget(&(data->sched), rsvid);
            break;
        case RTS_REMAINING_BUDGET:
            rep.rep_type = RTS_RSV_QUERY_OK;
            rep.payload = rts_scheduler_rsv_rem_budget(&(data->sched), rsvid);
            break;
        default:
            rep.rep_type = RTS_RSV_QUERY_ERR;
    }
        
    return rep;
}

static struct rts_reply req_rsv_destroy(struct rts_daemon* data, rsv_t rsvid) {
    struct rts_reply rep;
    
    if(rts_scheduler_rsv_destroy(&(data->sched), rsvid) < 0)
        rep.rep_type = RTS_RSV_DESTROY_ERR;
    else
        rep.rep_type = RTS_RSV_DESTROY_OK;
    
    return rep;
}

void rts_daemon_init(struct rts_daemon* data) {
    rts_carrier_init(&(data->chann));
    rts_taskset_init(&(data->tasks));
    rts_scheduler_init(&(data->sched), read_rt_kernel_budget());
}

void rts_daemon_register_sig(void (*func)(int)) {
    signal(SIGINT, func);
}

int rts_daemon_check_for_fail(struct rts_daemon* data, int cli_id) {
    struct rts_client* client;
    
    client = rts_carrier_get_client(&(data->chann), cli_id);
    
    if(client->state != ERROR && client->state != DISCONNECTED)
        return 0;
    
    rts_carrier_set_state(&(data->chann), cli_id, EMPTY);
    rts_scheduler_delete(&(data->sched), client->pid);
    
    return 1;
}

int rts_daemon_check_for_update(struct rts_daemon* data, int cli_id) {
    int is_updated;
    struct rts_client* client;
    
    client = rts_carrier_get_client(&(data->chann), cli_id);
    is_updated = rts_carrier_isupdated(&(data->chann), cli_id);
    
    if(client->state == CONNECTED && is_updated)
        return 1;
    
    return 0;
}

void rts_daemon_set_fail(struct rts_daemon* data, int cli_id) {
    struct rts_client* client;
    
    client = rts_carrier_get_client(&(data->chann), cli_id);
    client->state = ERROR;
}

void rts_daemon_handle_req(struct rts_daemon* data, int cli_id) {
    int sent;
        
    if(rts_daemon_check_for_fail(data, cli_id))
        return;
    
    if(!rts_daemon_check_for_update(data, cli_id))
        return;
       
    sent = rts_daemon_process_req(data, cli_id);

    if(sent <= 0)
        rts_daemon_set_fail(data, cli_id);    
}

int rts_daemon_process_req(struct rts_daemon* data, int cli_id) {
    struct rts_reply rep;
    struct rts_request req;
    
    req = rts_carrier_get_req(&(data->chann), cli_id);
    
    switch(req.req_type) {
        case RTS_CONNECTION:
            rep = req_connection(data, cli_id, req.payload.ids.pid);
            break;
        case RTS_CAP_QUERY:
            rep = req_cap_query(data, req.payload.query_type);
            break;
        case RTS_RSV_CREATE:
            rep = req_rsv_create(data, &(req.payload.param));
            break;
        case RTS_RSV_ATTACH:
            rep = req_rsv_attach(data, req.payload.ids.rsvid, req.payload.ids.pid);
        case RTS_RSV_DETACH:
            rep = req_rsv_detach(data, req.payload.ids.rsvid);
            break;
        case RTS_RSV_QUERY:
            rep = req_rsv_query(data, req.payload.ids.rsvid, req.payload.query_type);
            break;
        case RTS_RSV_DESTROY:
            rep = req_rsv_destroy(data, req.payload.ids.rsvid);
            break;
        default:
            rep.rep_type = RTS_REQUEST_ERR;
    }
    
    return rts_carrier_send(&(data->chann), &rep, cli_id);
}

void rts_daemon_loop(struct rts_daemon* data) {
    int i;

    while(1) {

        rts_carrier_update(&(data->chann));
        
        for(i = 0; i <= rts_carrier_get_conn(&(data->chann)); i++)
            rts_daemon_handle_req(data, i);
    }

    return;
}
