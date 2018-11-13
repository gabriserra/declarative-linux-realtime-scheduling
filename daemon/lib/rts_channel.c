#include "rts_channel.h"
#include <string.h>

// ACCESS ----

int rts_access_init(struct rts_access* c) {
    if(usocket_init(&(c->sock), TCP) < 0)
        return -1;
    if(usocket_timeout(&(c->sock), CHANNEL_TIMEOUT) < 0)
        return -1;
   
    return 0;
}

int rts_access_connect(struct rts_access* c) {
    return usocket_connect(&(c->sock), CHANNEL_PATH);
}

int rts_access_recv(struct rts_access* c) {
    return usocket_recv(&(c->sock), (void *)&(c->rep), sizeof(struct rts_reply));
}

int rts_access_send(struct rts_access* c) {
    return usocket_send(&(c->sock), (void *)&(c->req), sizeof(struct rts_request));
}

// CARRIER ----

int rts_carrier_init(struct rts_carrier* c) {
    memset(c, 0, sizeof(struct rts_access));

    if(usocket_init(&(c->sock), TCP) < 0) 
        return -1;

    if(usocket_bind(&(c->sock), CHANNEL_PATH) < 0)
        return -1;

    if(usocket_listen(&(c->sock), BACKLOG_MAX) < 0)
        return -1;

    return 0;
}

void rts_carrier_prepare(struct rts_carrier* c) {
    usocket_prepare_recv(&(c->sock));
}

int rts_carrier_get_conn(struct rts_carrier* c) {
    return usocket_get_maxfd(&(c->sock));
}

struct rts_client* rts_carrier_get_client(struct rts_carrier* c, int cli_id) {
    return &(c->client[cli_id]);
}

struct rts_request rts_carrier_get_req(struct rts_carrier* c, int cli_id) {
    return c->last_req[cli_id];
}

enum CLIENT_STATE rts_carrier_get_state(struct rts_carrier* c, int cli_id) {
    return c->client[cli_id].state;
}

void rts_carrier_set_state(struct rts_carrier* c, int cli_id, enum CLIENT_STATE s) {
    c->client[cli_id].state = s;
}

void rts_carrier_set_pid(struct rts_carrier* c, int cli_id, pid_t pid) {
    c->client[cli_id].pid = pid;
}

void rts_carrier_update(struct rts_carrier* c) {
    int i, n;

    n = usocket_get_maxfd(&(c->sock));
    memset(&(c->last_n), 0, n+1);
    usocket_recvall(&(c->sock), (void*)&(c->last_req), (int*)&(c->last_n), sizeof(struct rts_request));
    
    for(i = 0; i <= n; i++) {
        if(c->client[i].state == EMPTY && c->last_n[i] == 0)
            continue;
        else if(c->client[i].state == EMPTY && c->last_n[i] != 0)
            c->client[i].state = CONNECTED;
        else if(c->last_n[i] < 0)
            c->client[i].state = ERROR;
        else
            c->client[i].state = DISCONNECTED;   
    }
}

int rts_carrier_isupdated(struct rts_carrier* c, int cli_id) {
    if(c->last_n[cli_id] > 0)
        return 1;
    
    return 0;
}

void rts_carrier_setpid(struct rts_carrier* c, int cli_id, pid_t pid) {
    c->client[cli_id].pid = pid;
}

void rts_carrier_rm_conn(struct rts_access* cd) {
    return;
}

int rts_carrier_send(struct rts_carrier* c, struct rts_reply* r, int cli_id) {
    return usocket_sendto(&(c->sock), (void*)r, sizeof(struct rts_reply), cli_id);
}
