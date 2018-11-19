#ifndef RTS_LIB_H
#define RTS_LIB_H

#include "../daemon/lib/rts_channel.h"
#include <time.h>

#define REACTIVITY 0.5

#define DEADLINE_TO_PERIOD(dl) dl * 2
#define WCET_TO_PERIOD(wcet) wcet * 3

struct rts_thread {
    uint32_t t_num;
    uint32_t t_period;
    uint32_t t_wcet;
    uint32_t t_activation_num_tot;
    uint32_t t_activation_num_curr;
    struct timespec t_activation_time;
};

int rts_daemon_connect(struct rts_access* c);

int rts_refresh_sys(struct rts_access* c);

int rts_refresh_single(struct rts_access* c, rsv_t rsvid);

float rts_cap_query(struct rts_access* c, enum QUERY_TYPE type);

int rts_params_init(struct rts_params *tp);

void rts_set_clock(struct rts_params* tp, clockid_t clk);

clockid_t rts_get_clock(struct rts_params* tp);

void rts_set_period(struct rts_params* tp, uint32_t period);

void rts_set_budget(struct rts_params* tp, uint32_t budget);

void rts_set_deadline(struct rts_params* tp, uint32_t deadline);

void rts_set_priority(struct rts_params* tp, uint32_t priority);

void rts_params_cleanup(struct rts_params* tp);

uint64_t rts_params_get_est_param(struct rts_params* tp, int FLAG);

int rts_create_rsv(struct rts_access* c, struct rts_params* tp, rsv_t* id);

int rts_rsv_attach_thread(struct rts_access* c, rsv_t id, pid_t pid);

int rts_rsv_detach_thread(struct rts_access* c, rsv_t id);

int rts_rsv_get_remaining_budget(struct rts_access* c, rsv_t id, float* budget);

int rts_rsv_destroy(struct rts_access* c, rsv_t id);

int rts_daemon_deconnect(struct rts_access* c);

void rts_rsv_begin(struct rts_params* tp);

void rts_rsv_end(struct rts_params* tp);

// THREAD _ TEST

void rts_thread_init(struct rts_thread* t, struct rts_params* p);

void rts_thread_compute(struct rts_thread* t);

void rts_thread_wait_activation(struct rts_thread* t);

void rts_thread_calc_exec(struct rts_thread* t, int cfg_budg, int cfg_per, int cfg_dead);

void rts_thread_calc_period(struct rts_thread* t, int cfg_budg, int cfg_per, int cfg_dead);

void rts_thread_rand_activation_num(struct rts_thread* t);

void rts_thread_set_activation_num(struct rts_thread* t, int curr, int tot);

int rts_thread_computation_ended(struct rts_thread* t);

void rts_thread_print_info(struct rts_thread* t, struct rts_params* p, int tid);

float rts_thread_calc_budget(struct rts_thread* t, struct rts_params* p);

float rts_thread_calc_rem_budget(struct rts_thread* t, struct rts_params* p);

#endif	// RTS_LIB_H

