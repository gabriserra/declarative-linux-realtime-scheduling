/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

#include "timeutils.h"

void time_add_us(struct timespec *t, uint64_t us) {
    t->tv_sec += MICRO_TO_SEC(us);               
    t->tv_nsec += MICRO_TO_NANO(us % EXP6);     
	
    if (t->tv_nsec > EXP9) { 
        t->tv_nsec -= EXP9; 
        t->tv_sec += 1;
    }
}

void time_add_ms(struct timespec *t, uint32_t ms) {
    t->tv_sec += MILLI_TO_SEC(ms);
    t->tv_nsec += MILLI_TO_NANO(ms % EXP3);
	
    if (t->tv_nsec > EXP9) { 
        t->tv_nsec -= EXP9; 
        t->tv_sec += 1;
    }
}

int time_cmp(struct timespec* t1, struct timespec* t2) {
    if (t1->tv_sec > t2->tv_sec) 
            return 1; 
    if (t1->tv_sec < t2->tv_sec) 
            return -1;

    if (t1->tv_nsec > t2->tv_nsec) 
            return 1; 
    if (t1->tv_nsec < t2->tv_nsec) 
            return -1; 

    return 0;
}

void time_copy(struct timespec* td, struct timespec* ts) {
    td->tv_sec  = ts->tv_sec;
    td->tv_nsec = ts->tv_nsec;
}

struct timespec get_time_now(clockid_t clk) {
    struct timespec ts;
    
    clock_gettime(clk, &ts);
    return ts;
}

void compute_for(struct timespec* t_init, uint32_t exec_milli_max) {
    uint32_t exec_milli;
    struct timespec t_curr;
    struct timespec t_end;
    
    exec_milli = rand() % exec_milli_max;
    time_copy(&t_end, t_init);
    time_add_ms(&t_end, exec_milli);
    
    while(1) {
        __asm__ ("nop"); // simulate computation of something..
        clock_gettime(CLOCK_MONOTONIC, &t_curr);
        
        if(time_cmp(&t_curr, &t_end) > 0)
            break;
    } 
}

void wait_next_activation(struct timespec* t_act, uint32_t period_milli) {
    time_add_ms(t_act, period_milli);
    clock_nanosleep(CLOCK_MONOTONIC, TIMER_ABSTIME, t_act, NULL);
}

int computation_ended(int* curr_num_activation, int max_num_activation) {    
    if(*curr_num_activation < max_num_activation)
        (*curr_num_activation)++;
    else
        return 1;
    
    return 0;
}

