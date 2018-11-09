/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

/* 
 * File:   threadconfutils.h
 * Author: gabrieleserra
 *
 * Created on November 8, 2018, 11:06 AM
 */

#ifndef THREADCONFUTILS_H
#define THREADCONFUTILS_H

#define COLUMN_MAX  80      // used to par

#define DEADLINE_TO_PERIOD(dl) dl * 2
#define WCET_TO_PERIOD(wcet) wcet * 3

#define THREAD_CLK CLOCK_MONOTONIC

// DATA STRUCT

struct thread_params {
    int num;
    struct rts_params* par;
};

void conf_threads(char* filename, int nthread, struct rts_params* p);

void skip_comment(FILE* f);

void fill_params(struct thread_params* t_par, int num, struct rts_params* rt_par);

uint32_t calc_exec(int cfg_budg, int cfg_per, int cfg_dead);

uint32_t calc_period(int cfg_budg, int cfg_per, int cfg_dead);

uint32_t calc_activation_num();

#endif /* THREADCONFUTILS_H */

