#include "rts_utils.h"
#include <stdlib.h>

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

uint64_t timespec_to_us(struct timespec *t) {
    uint64_t us;
    
    us = SEC_TO_MICRO(t->tv_sec);
    us += NANO_TO_MICRO(t->tv_nsec);
	
    return us;
}

void us_to_timespec(struct timespec *t, uint64_t us) {
    t->tv_sec = MICRO_TO_SEC(us);               
    t->tv_nsec = MICRO_TO_NANO(us % EXP6);
}

uint32_t timespec_to_ms(struct timespec *t) {
    uint32_t ms;
    
    ms = SEC_TO_MILLI(t->tv_sec);
    ms += NANO_TO_MILLI(t->tv_nsec);
	
    return ms;
}

void ms_to_timespec(struct timespec *t, uint32_t ms) {
    t->tv_sec = MILLI_TO_SEC(ms);               
    t->tv_nsec = MILLI_TO_NANO(ms % EXP3);
}

struct timespec get_time_now(clockid_t clk) {
    struct timespec ts;
    
    clock_gettime(clk, &ts);
    return ts;
}

void get_time_now2(clockid_t clk, struct timespec* t) {
    clock_gettime(clk, t);
}

uint32_t get_time_now_ms(clockid_t clk) {
    struct timespec ts;
    
    clock_gettime(clk, &ts);
    return timespec_to_ms(&ts);
}

struct timespec get_thread_time() {
    struct timespec ts;
    
    clock_gettime(CLOCK_THREAD_CPUTIME_ID, &ts);
    return ts;
}

uint32_t get_thread_time_ms() {
    struct timespec ts;
    
    clock_gettime(CLOCK_THREAD_CPUTIME_ID, &ts);
    return timespec_to_ms(&ts);
}

void compute_for(struct timespec* t_act, uint32_t exec_milli_max) {
    uint32_t exec_milli;
    struct timespec t_curr;
    struct timespec t_end;
    
    exec_milli = rand() % exec_milli_max; 
    time_copy(&t_end, t_act);
    time_add_ms(&t_end, exec_milli);
    
    while(1) {
        __asm__ ("nop"); // simulate computation of something..
        clock_gettime(CLOCK_THREAD_CPUTIME_ID, &t_curr);
        
        if(time_cmp(&t_curr, &t_end) > 0)
            break;
    } 
}

void wait_next_activation(struct timespec* t_act, uint32_t period_milli) {
    time_add_ms(t_act, period_milli);
    clock_nanosleep(CLOCK_MONOTONIC, TIMER_ABSTIME, t_act, NULL);
}