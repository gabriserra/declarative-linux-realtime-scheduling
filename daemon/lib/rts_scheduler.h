#ifndef RTS_SCHEDULER_H
#define RTS_SCHEDULER_H

#include "rts_types.h"
#include <sys/types.h>

struct rts_taskset;
struct rts_plugin;
struct rts_task;

struct rts_scheduler {    
    int num_of_plugin;
    int num_of_cpu;
    long next_rsv_id;
    int sys_rt_runtime;
    int sys_rt_period;
    float* sys_rt_free_utils;
    float* sys_rt_curr_free_utils;
    struct rts_taskset* taskset;
    struct rts_plugin* plugin;
};

void rts_scheduler_init(struct rts_scheduler* s, struct rts_taskset* ts, int rt_period, int rt_runtime);

void rts_scheduler_destroy(struct rts_scheduler* s);

void rts_scheduler_delete(struct rts_scheduler* s, pid_t pid);

int rts_scheduler_refresh_utils(struct rts_scheduler* s);

void rts_scheduler_refresh_prios(struct rts_scheduler* s);

int rts_scheduler_refresh_util(struct rts_scheduler* s, struct rts_task* t);

void rts_scheduler_refresh_prio(struct rts_scheduler* s, struct rts_task* t);

float rts_scheduler_get_free_util(struct rts_scheduler* s);

float rts_scheduler_get_remaining_util(struct rts_scheduler* s);

rsv_t rts_scheduler_rsv_create(struct rts_scheduler* s, struct rts_params* tp, pid_t ppid);

int rts_scheduler_rsv_attach(struct rts_scheduler* s, rsv_t rsvid, pid_t pid);

int rts_scheduler_rsv_detach(struct rts_scheduler* s, rsv_t rsvid);

float rts_scheduler_rsv_budget(struct rts_scheduler* s, rsv_t rsvid);

float rts_scheduler_rsv_rem_budget(struct rts_scheduler* s, rsv_t rsvid);

int rts_scheduler_rsv_destroy(struct rts_scheduler* s, rsv_t rsvid);

#endif	// RTS_SCHEDULER_H

