#ifndef RTS_SCHEDULER_H
#define RTS_SCHEDULER_H

#include "rts_types.h"
#include <sys/types.h>

struct rts_taskset;
struct rts_plugin;

struct rts_scheduler {    
    int num_of_plugin;
    float sys_rt_budget;
    long next_rsv_id;
    struct rts_taskset* taskset;
    struct rts_plugin* plugin;
};

void rts_scheduler_init(struct rts_scheduler* s, struct rts_taskset* ts, float sys_rt_budget);

void rts_scheduler_delete(struct rts_scheduler* s, pid_t pid);

float rts_scheduler_remaining_budget(struct rts_scheduler* s);

rsv_t rts_scheduler_rsv_create(struct rts_scheduler* s, struct rts_params* tp, pid_t ppid);

int rts_scheduler_rsv_attach(struct rts_scheduler* s, rsv_t rsvid, pid_t pid);

int rts_scheduler_rsv_detach(struct rts_scheduler* s, rsv_t rsvid);

float rts_scheduler_rsv_budget(struct rts_scheduler* s, rsv_t rsvid);

float rts_scheduler_rsv_rem_budget(struct rts_scheduler* s, rsv_t rsvid);

int rts_scheduler_rsv_destroy(struct rts_scheduler* s, rsv_t rsvid);

#endif	// RTS_SCHEDULER_H

