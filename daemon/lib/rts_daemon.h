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

struct rts_daemon {
    struct rts_carrier chann;
    struct rts_scheduler sched;
    struct rts_taskset tasks;
};

void rts_daemon_init(struct rts_daemon* data);

void rts_daemon_register_sig(void (*func)(int));

void rts_daemon_handle_req(struct rts_daemon* data, int cli_id);

int rts_daemon_process_req(struct rts_daemon* data, int cli_id);

void rts_daemon_loop(struct rts_daemon* data);

#endif /* REQUESTUTILS_H */

