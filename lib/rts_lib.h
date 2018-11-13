#ifndef RTS_LIB_H
#define RTS_LIB_H

#include "../daemon/lib/rts_channel.h"
#include <time.h>

#define REACTIVITY 0.5

int rts_daemon_connect(struct rts_access* c);

float rts_cap_query(struct rts_access* c, enum QUERY_TYPE type);

int rts_params_init(struct rts_params *tp);

void rts_set_clock(struct rts_params* tp, clockid_t clk);

clockid_t rts_get_clock(struct rts_params* tp);

void rts_set_period(struct rts_params* tp, uint32_t period);

void rts_set_budget(struct rts_params* tp, uint32_t budget);

void rts_set_deadline(struct rts_params* tp, uint32_t deadline);

void rts_set_priority(struct rts_params* tp, uint32_t priority);

void rts_params_cleanup(struct rts_params* tp);

int rts_create_rsv(struct rts_access* c, struct rts_params* tp, rsv_t* id);

int rts_rsv_attach_thread(struct rts_access* c, rsv_t id, pid_t pid);

int rts_rsv_detach_thread(struct rts_access* c, rsv_t id);

int rts_rsv_get_remaining_budget(struct rts_access* c, rsv_t id, float* budget);

int rts_rsv_destroy(struct rts_access* c, rsv_t id);

int rts_daemon_deconnect(struct rts_access* c);

void rts_rsv_begin(struct rts_params* tp);

void rts_rsv_end(struct rts_params* tp);

#endif	// RTS_LIB_H

