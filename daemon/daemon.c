// TODO -> sostituire tutte le printf con syslog

#include "../lib/rts_taskset.h"
#include "../lib/rts_channel.h"
#include "../lib/rts_scheduler.h"
#include "../lib/rts_utils.h"
#include <stdlib.h>
#include <stdio.h>
#include <signal.h>
#include <string.h>

void handle_req(int cli_id);

struct rts_deamon {
    struct rts_carrier chann;
    struct rts_scheduler sched;
    struct rts_taskset tasks;
} data;

void daemon_term() {
    printf("Killed..\n");
    exit(EXIT_SUCCESS);
}

void daemon_init() {
    rts_carrier_init(&(data.chann));
    rts_taskset_init(&(data.tasks));
    //rts_scheduler_init(&(data.sched), read_rt_kernel_budget());
}

void daemon_loop() {
    int i;

    while(1) {

        printf("In attesa di aggiornamenti..\n");
        rts_carrier_update(&(data.chann));
        
        for(i = 0; i <= rts_carrier_get_conn(&(data.chann)); i++)
            handle_req(i);
    }

    return;
}

int main(int argc, char* argv[]) {
    printf("rtsdaemond - Daemon started.\n");
    
    signal(SIGINT, daemon_term);
    daemon_init();
    daemon_loop();
}



void handle_req(int cli_id) {
    struct rts_client* client;
    struct rts_request* req;
    struct rts_reply rep;
    int is_updated, n;
    enum CLIENT_STATE state;

    client = rts_carrier_get_client(&(data.chann), cli_id);
    
    if(client->state == ERROR || client->state == DISCONNECTED) {
        rts_carrier_set_state(&(data.chann), cli_id, EMPTY);
        rts_scheduler_delete(&(data.sched), client->pid);
        return;
    }

    is_updated = rts_carrier_isupdated(&(data.chann), cli_id);

    if(client->state == CONNECTED && is_updated) {
        req = rts_carrier_get_req(&(data.chann), cli_id);
        process_req(req, &rep, cli_id);
        n = rts_carrier_send(&(data.chann), &rep, cli_id);

        if(n < 0)
            client->state = ERROR;
    }
    
}

void process_req(struct rts_request* req, struct rts_reply* rep, int cli_id) {
    switch(req->req_type) {
        case RTS_CONNECTION:
            rts_carrier_setpid(&(data.chann), cli_id, req->payload.ids.pid);
            rep->rep_type = RTS_CONNECTION_OK;
            break;
        case RTS_CAP_QUERY:
            if(req->payload.query_type == RTS_BUDGET) {
                rep->payload = read_rt_kernel_budget();
                rep->rep_type = RTS_CAP_QUERY_OK;
            } else if (req->payload.query_type == RTS_REMAINING_BUDGET) {
                rep->payload = rts_scheduler_get_remaining_budget(&(data.sched));
                rep->rep_type = RTS_CAP_QUERY_OK;
            } else {
                rep->rep_type = RTS_CAP_QUERY_OK;
            }
            break;
        case RTS_RSV_CREATE:
            int rsvid = rts_scheduler_create(&(data.sched), &(req->payload.param), cli_id);
            if(rsvid < 0) {
                rep->rep_type = RTS_RSV_CREATE_ERR;
                rep->payload = -1;
            } else {
                rep->rep_type = RTS_RSV_CREATE_OK;
                rep->payload = rsvid;
            }
            break;
        case RTS_RSV_ATTACH:
            if(rts_scheduler_attach(&(data.sched), cli_id, req->payload.ids.rsvid, req->payload.ids.pid) < 0)
                rep->rep_type = RTS_RSV_ATTACH_ERR;
            else
                rep->rep_type = RTS_RSV_ATTACH_OK;
            break;
        case RTS_RSV_DETACH:
            if(rts_scheduler_detach(&(data.sched), cli_id, req->payload.ids.rsvid, req->payload.ids.pid) < 0)
                rep->rep_type = RTS_RSV_ATTACH_ERR;
            else
                rep->rep_type = RTS_RSV_ATTACH_OK;
            break;
        case RTS_RSV_QUERY:
            if(req->payload.query_type == RTS_BUDGET) {
                rep->rep_type = RTS_RSV_QUERY_OK;
                rep->payload = rts_scheduler_get_task_budget(&(data.sched), req->payload.ids.rsvid);
            } else if(req->payload.query_type == RTS_REMAINING_BUDGET) {
                rep->rep_type = RTS_RSV_QUERY_OK;
                rep->payload = rts_scheduler_get_task_rem_budget(&(data.sched), req->payload.ids.rsvid);
            } else {
                rep->rep_type = RTS_RSV_QUERY_ERR;
            }
            break;
        case RTS_RSV_DESTROY:
            if(rts_scheduler_destroy_rsv(&(data.sched), req->payload.ids.rsvid) < 0)
                rep->rep_type = RTS_RSV_DESTROY_ERR;
            else
                rep->rep_type = RTS_RSV_DESTROY_OK;
            break;
        default:
            rep->rep_type = RTS_REQUEST_ERR;
    }
}