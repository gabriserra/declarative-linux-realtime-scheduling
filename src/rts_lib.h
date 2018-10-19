#include "lib/rts_types.h"
#include "lib/rts_channel.h"

int rts_daemon_connect(struct rts_channel* c);

float rts_cap_query(struct rts_channel* c, enum QUERY_TYPE type);

void rts_params_init(struct rts_params *tp);

void rts_set_period(struct rts_params* tp, uint32_t period);

void rts_set_budget(struct rts_params* tp, uint32_t budget);

void rts_set_deadline(struct rts_params* tp, uint32_t deadline);

void rts_set_priority(struct rts_params* tp, uint32_t priority);

int rts_create_rsv(struct rts_channel* c, struct rts_params* tp, rsv_t* id);

int rts_rsv_attach_thread(struct rts_channel* c, rsv_t id, pid_t pid);

int rts_rsv_detach_thread(struct rts_channel* c, rsv_t id);