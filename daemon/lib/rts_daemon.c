/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

#include "../components/logger.h"
#include "rts_daemon.h"
#include "rts_utils.h"
#include <stdlib.h>
#include <signal.h>
#include <stdio.h>


void remove_rt_kernel_limit(int* rt_period, int* rt_runtime) {
    FILE* proc_rt_period = fopen(PROC_RT_PERIOD_FILE, "r");
    FILE* proc_rt_runtime = fopen(PROC_RT_RUNTIME_FILE, "r");

    if(proc_rt_period == NULL || proc_rt_runtime == NULL) {
        LOG("Error during proc file open ...\n");
        exit(EXIT_FAILURE);
    }

    fscanf(proc_rt_period, "%d", rt_period);
    fscanf(proc_rt_runtime, "%d", rt_runtime);
    
    LOG("Kernel RT data - PERIOD: %d - RUNTIME: %d\n", *rt_period, *rt_runtime);
    
    freopen(PROC_RT_RUNTIME_FILE, "w", proc_rt_period);
    fprintf(proc_rt_period, "%d", -1);
    
    LOG("RT limit removed. They will be restored at daemon execution end.\n");
    
    fclose(proc_rt_period);
    fclose(proc_rt_runtime);
}

void restore_rt_kernel_limit(int rt_runtime) {
    FILE* proc_rt_runtime = fopen(PROC_RT_RUNTIME_FILE, "w");

    if(proc_rt_runtime == NULL) {
        LOG("Error during proc file open ...\n");
        exit(EXIT_FAILURE);
    }

    fprintf(proc_rt_runtime, "%d", rt_runtime);
    fclose(proc_rt_runtime);
}

static struct rts_reply req_connection(struct rts_daemon* data, int cli_id, pid_t ppid) {
    struct rts_reply rep;
    
    LOG("Received CONNECTION REQ from pid: %d\n", ppid);
    rts_carrier_set_pid(&(data->chann), cli_id, ppid);
    rep.rep_type = RTS_CONNECTION_OK;
    LOG("%d connected with success. Assigned id: %d\n", ppid, cli_id);
    
    return rep;
}

static struct rts_reply req_refresh_sys(struct rts_daemon* data) {
    int cpu_overl;
    struct rts_reply rep;
    
    LOG("Received REFRESH_SYS REQ. Task performance will be re-evaluated.\n");
    cpu_overl = rts_scheduler_refresh_utils(&(data->sched));
    rts_scheduler_refresh_prios(&(data->sched));
    
    if(cpu_overl < 0)
        rep.rep_type = RTS_REFRESH_SYS_OVL;
    else   
        rep.rep_type = RTS_REFRESH_SYS_OK;
    
    rep.payload = cpu_overl * -1;
    LOG("Performance evaluation complete. Number of CPU overloaded: %f\n", rep.payload);
    
    return rep;
}

static struct rts_reply req_refresh_single(struct rts_daemon* data, rsv_t rsvid) {
    struct rts_reply rep;
    struct rts_task* t;
    
    LOG("Received RFRESH_SINGLE REQ for %d reservation.\n", rsvid);
    t = rts_taskset_search(data->sched.taskset, rsvid);
    
    if(t == NULL) {
        rep.rep_type = RTS_REFRESH_SINGLE_ERR;
        return rep;
    }
            
    if(rts_scheduler_refresh_util(&(data->sched), t) < 0)
        rep.rep_type = RTS_REFRESH_SINGLE_OVL;
    else   
        rep.rep_type = RTS_REFRESH_SINGLE_OK;
    
    rts_scheduler_refresh_prio(&(data->sched), t);
    return rep;
}

static struct rts_reply req_cap_query(struct rts_daemon* data, enum QUERY_TYPE type) {
    struct rts_reply rep;
    
    switch(type) {
        case RTS_BUDGET:
            LOG("Received CAP_BUDGET REQ.\n");
            rep.payload = rts_scheduler_get_free_util(&(data->sched));
            rep.rep_type = RTS_CAP_QUERY_OK;
            LOG("System default free utilization: %f\n", rep.payload);
            break;
        case RTS_REMAINING_BUDGET:
            LOG("Received CAP_REMAINING_BUDGET REQ.\n");
            rep.payload = rts_scheduler_get_remaining_util(&(data->sched));
            rep.rep_type = RTS_CAP_QUERY_OK;
            LOG("System average remaining utilization: %f\n", rep.payload);
            break;
        default:
            LOG("Received invalid CAP REQ.\n");
            rep.rep_type = RTS_CAP_QUERY_ERR;
    }
    
    return rep;
}

static struct rts_reply req_rsv_create(struct rts_daemon* data, struct rts_params* p, pid_t ppid) {
    int rsv_id;
    struct rts_reply rep;
    
    LOG("Received RSV_CREATE REQ from pid: %d\n", ppid);
    rsv_id = rts_scheduler_rsv_create(&(data->sched), p, ppid);
            
    if(rsv_id < 0) {
        rep.rep_type = RTS_RSV_CREATE_ERR;
        rep.payload = -1;
        LOG("It is NOT possible to guarantee these parameters!\n");
    } else {
        rep.rep_type = RTS_RSV_CREATE_OK;
        rep.payload = rsv_id;
         LOG("It is possible to guarantee these parameters. Res. id: %d\n", rsv_id);
    }
    
    return rep;
}

static struct rts_reply req_rsv_attach(struct rts_daemon* data, rsv_t rsvid, pid_t pid) {
    struct rts_reply rep;
    
    LOG("Received RSV_ATTACH REQ for res: %d. PID: %d will be attached.\n", rsvid, pid);
    
    if(rts_scheduler_rsv_attach(&(data->sched), rsvid, pid) < 0)
        rep.rep_type = RTS_RSV_ATTACH_ERR;
    else
        rep.rep_type = RTS_RSV_ATTACH_OK;
    
    return rep;
}

static struct rts_reply req_rsv_detach(struct rts_daemon* data, rsv_t rsvid) {
    struct rts_reply rep;
    
    LOG("Received RSV_DETACH REQ for res: %d. The thread will be detached\n", rsvid);
    
    if(rts_scheduler_rsv_detach(&(data->sched), rsvid) < 0)
        rep.rep_type = RTS_RSV_DETACH_ERR;
    else
        rep.rep_type = RTS_RSV_DETACH_OK;
    
    return rep;   
}

/*
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
*/

static struct rts_reply req_rsv_destroy(struct rts_daemon* data, rsv_t rsvid) {
    struct rts_reply rep;
    
    LOG("Received RSV_DESTROY REQ for res: %d. The thread will be detached\n", rsvid);
    
    if(rts_scheduler_rsv_destroy(&(data->sched), rsvid) < 0)
        rep.rep_type = RTS_RSV_DESTROY_ERR;
    else
        rep.rep_type = RTS_RSV_DESTROY_OK;
    
    return rep;
}

int rts_daemon_init(struct rts_daemon* data) {
    int rt_period;
    int rt_runtime; 
    
    remove_rt_kernel_limit(&rt_period, &rt_runtime);
    
    if(rts_carrier_init(&(data->chann)) < 0)
        return -1;
    
    rts_taskset_init(&(data->tasks));
    rts_scheduler_init(&(data->sched), &(data->tasks), rt_period, rt_runtime);
        
    return 0;
}

void rts_daemon_register_sig_int(void (*func)(int)) {
    signal(SIGINT, func);
}

void rts_daemon_register_sig_alarm(void (*func)(int)) {
    signal(SIGALRM, func);
}

int rts_daemon_check_for_fail(struct rts_daemon* data, int cli_id) {
    struct rts_client* client;
    
    client = rts_carrier_get_client(&(data->chann), cli_id);
    
    if(client->state != ERROR && client->state != DISCONNECTED)
        return 0;
    
    LOG("Client %d disconnected. Its reservation will be destroyed.\n", cli_id);
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
    struct rts_client* client;
    
    client = rts_carrier_get_client(&(data->chann), cli_id);
    req = rts_carrier_get_req(&(data->chann), cli_id);
    
    switch(req.req_type) {
        case RTS_CONNECTION:
            rep = req_connection(data, cli_id, req.payload.ids.pid);
            break;
        case RTS_REFRESH_SYS:
            rep = req_refresh_sys(data);
            break;
        case RTS_REFRESH_SINGLE:
            rep = req_refresh_single(data, req.payload.ids.rsvid);
            break;
        case RTS_CAP_QUERY:
            rep = req_cap_query(data, req.payload.query_type);
            break;
        case RTS_RSV_CREATE:
            rep = req_rsv_create(data, &(req.payload.param), client->pid);
            break;
        case RTS_RSV_ATTACH:
            rep = req_rsv_attach(data, req.payload.ids.rsvid, req.payload.ids.pid);
            break;
        case RTS_RSV_DETACH:
            rep = req_rsv_detach(data, req.payload.ids.rsvid);
            break;
        //case RTS_RSV_QUERY:
            //rep = req_rsv_query(data, req.payload.ids.rsvid, req.payload.query_type);
            //break;
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
    
    rts_carrier_prepare(&(data->chann));

    while(1) {

        rts_carrier_update(&(data->chann));
        
        for(i = 0; i <= rts_carrier_get_conn(&(data->chann)); i++)
            rts_daemon_handle_req(data, i);
    }

    return;
}

void rts_daemon_destroy(struct rts_daemon* data) {
    struct rts_task* t;
        
    while(1) {
        t = rts_taskset_remove_top(&(data->tasks));
        
        if(t == NULL)
            break;
        
        rts_task_destroy(t);
    }
    
    restore_rt_kernel_limit(data->sched.sys_rt_runtime);
    rts_scheduler_destroy(&(data->sched));
}
