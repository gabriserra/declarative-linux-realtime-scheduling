/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

/* 
 * File:   timeutils.h
 * Author: gabrieleserra
 *
 * Created on November 8, 2018, 10:01 AM
 */

#ifndef TIMEUTILS_H
#define TIMEUTILS_H

#include <time.h>
#include <stdint.h>

#define EXP3 1000
#define EXP6 1000000
#define EXP9 1000000000

#define SEC_TO_MILLI(sec) sec * EXP3
#define SEC_TO_MICRO(sec) sec * EXP6
#define SEC_TO_NANO(sec) sec * EXP9

#define MILLI_TO_SEC(milli) milli / EXP3
#define MILLI_TO_MICRO(milli) milli * EXP3
#define MILLI_TO_NANO(milli) milli * EXP6

#define MICRO_TO_SEC(micro) micro / EXP6
#define MICRO_TO_MILLI(micro) micro / EXP3
#define MICRO_TO_NANO(micro) micro * EXP3

#define NANO_TO_SEC(nano) nano / EXP9
#define NANO_TO_MILLI(nano) nano / EXP6
#define NANO_TO_MICRO(nano) nano / EXP3

void time_add_us(struct timespec *t, uint64_t us);

void time_add_ms(struct timespec *t, uint32_t ms);

int time_cmp(struct timespec* t1, struct timespec* t2);

void time_copy(struct timespec* td, struct timespec* ts);

uint32_t timespec_to_ms(struct timespec *t);

struct timespec get_time_now(clockid_t clk);

uint32_t get_time_now_ms(clockid_t clk);

void compute_for(struct timespec* t_init, uint32_t exec_milli_max);

void wait_next_activation(struct timespec* t_act, uint32_t period_milli);

int computation_ended(int* curr_num_activation, int max_num_activation);

#endif /* TIMEUTILS_H */

