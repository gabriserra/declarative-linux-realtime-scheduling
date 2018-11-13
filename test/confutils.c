/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

#include "confutils.h"
#include "../lib/rts_lib.h"
#include <stdlib.h>

static void exit_err(char* strerr) {
    printf("FATAL: %s\n", strerr);
    exit(EXIT_FAILURE);
}

void conf_threads(char* filename, int nthread, struct rts_params* p) {
    FILE* f;
    uint32_t priority, budget, deadline, period;
            
    f = fopen(filename, "r");
    
    if(f == NULL)
        exit_err("Error while opening the file.\n");

    skip_comment(f);
    
    for(int i = 0; i < nthread; i++) {
        rts_params_init(&(p[i]));
        
        rts_set_clock(&(p[i]), CLOCK_MONOTONIC);
        
        fscanf(f, "%d", &priority);
        if(priority != 0)
            rts_set_priority(&(p[i]), priority);
        
        fscanf(f, "%d", &budget);
        if(budget != 0)
            rts_set_budget(&(p[i]), budget);
                
        fscanf(f, "%d", &deadline);
        if(deadline != 0)
            rts_set_deadline(&(p[i]), deadline);
                    
        fscanf(f, "%d", &period);
        if(period != 0)
            rts_set_period(&(p[i]), period);
    }    
}

void skip_comment(FILE* f) {
    char buffer[COLUMN_MAX];
    
    while(1) {
        fgets(buffer, COLUMN_MAX, f);
        
        if(buffer[0] != '#')
            break;
    } 
}

void fill_params(struct thread_params* t_par, int num, struct rts_params* par) {
    t_par->num = num;
    t_par->par = par;
}

uint32_t calc_exec(int cfg_budg, int cfg_per, int cfg_dead) {
    if(cfg_budg != 0)
        return cfg_budg;
    else if(cfg_dead != 0)
        return cfg_dead;
    else if(cfg_per != 0)
        return cfg_per;
    else
        return rand();
}

uint32_t calc_period(int cfg_budg, int cfg_per, int cfg_dead) {
    if(cfg_per != 0)
        return cfg_per;
    else if(cfg_dead != 0)
        return DEADLINE_TO_PERIOD(cfg_dead);
    else if(cfg_budg != 0)
        return WCET_TO_PERIOD(cfg_budg);
    else
        return rand();   
}

uint32_t calc_activation_num() {
    return rand();
}