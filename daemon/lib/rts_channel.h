#ifndef RTS_CHANNEL_H
#define RTS_CHANNEL_H

#include "rts_types.h"
#include "../components/usocket.h"

#define CHANNEL_PATH_CARRIER "/tmp/channel"
#define CHANNEL_PATH_ACCESS "/tmp/channel"
#define CHANNEL_MAX_SIZE SET_MAX_SIZE
#define CHANNEL_TIMEOUT 150

struct rts_access {
    struct usocket sock;
    struct rts_request req;
    struct rts_reply rep;
};

struct rts_carrier {
    struct usocket sock;
    int last_n[CHANNEL_MAX_SIZE];
    struct rts_request last_req[CHANNEL_MAX_SIZE];
    struct rts_client client[CHANNEL_MAX_SIZE];
};

// ACCESS ----

int rts_access_init(struct rts_access* c);

int rts_access_connect(struct rts_access* c);

int rts_access_recv(struct rts_access* c);

int rts_access_send(struct rts_access* c);

// CARRIER ---------

int rts_carrier_init(struct rts_carrier* c);

void rts_carrier_prepare(struct rts_carrier* c);

int rts_carrier_get_conn(struct rts_carrier* c);

void rts_carrier_update(struct rts_carrier* c);

void rts_carrier_select(struct rts_carrier* c, struct rts_request arr[CHANNEL_MAX_SIZE]);

int rts_carrier_isupdated(struct rts_carrier* c, int cli_id);

enum CLIENT_STATE rts_carrier_get_state(struct rts_carrier* c, int cli_id);

void rts_carrier_set_state(struct rts_carrier* c, int cli_id, enum CLIENT_STATE s);

void rts_carrier_set_pid(struct rts_carrier* c, int cli_id, pid_t pid);

int rts_carrier_get_size(struct rts_carrier* c);

void rts_carrier_recv(struct rts_carrier* c, struct rts_request* req, int i);

int rts_carrier_send(struct rts_carrier* c, struct rts_reply* rep, int i);

struct rts_request rts_carrier_get_req(struct rts_carrier* c, int cli_id);

struct rts_client* rts_carrier_get_client(struct rts_carrier* c, int cli_id);

#endif	// RTS_CHANNEL_H
