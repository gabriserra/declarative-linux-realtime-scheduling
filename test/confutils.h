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

#include <stdio.h>
#include <stdint.h>

#define COLUMN_MAX  80      // used to par
#define THREAD_CLK CLOCK_MONOTONIC_RAW

// DATA STRUCT

struct thread_params {
    int num;
    struct rts_params* par;
};

void conf_threads(char* filename, int nthread, struct rts_params* p);

void skip_comment(FILE* f);

void fill_params(struct thread_params* t_par, int num, struct rts_params* rt_par);

#endif /* THREADCONFUTILS_H */

