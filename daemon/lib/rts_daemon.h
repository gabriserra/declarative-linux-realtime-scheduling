/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

/* 
 * File:   requestutils.h
 * Author: gabrieleserra
 *
 * Created on November 8, 2018, 3:44 PM
 */

#ifndef RTS_DAEMON_H
#define RTS_DAEMON_H

#include "rts_taskset.h"
#include "rts_channel.h"
#include "rts_scheduler.h"

#define PROC_RT_PERIOD_FILE "/proc/sys/kernel/sched_rt_period_us"
#define PROC_RT_RUNTIME_FILE "/proc/sys/kernel/sched_rt_runtime_us"

struct rts_daemon {
    struct rts_carrier chann;
    struct rts_scheduler sched;
    struct rts_taskset tasks;
};

int rts_daemon_init(struct rts_daemon* data);

void rts_daemon_destroy(struct rts_daemon* data);

void rts_daemon_register_sig_int(void (*func)(int));

void rts_daemon_register_sig_alarm(void (*func)(int));

void rts_daemon_handle_req(struct rts_daemon* data, int cli_id);

int rts_daemon_process_req(struct rts_daemon* data, int cli_id);

void rts_daemon_loop(struct rts_daemon* data);

#endif /* REQUESTUTILS_H */

