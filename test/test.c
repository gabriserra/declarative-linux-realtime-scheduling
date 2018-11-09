#include "test.h"
#include "../lib/rts_lib.h"
#include <pthread.h>
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <sys/syscall.h>

/**
 * Shared data between threads and main process.
 */
pid_t* t_ids;
struct monitor m;

/**
 * Threads routine.
 * 
 */

void* RT_task(void* argv) 
{
    int t_num;
    int t_period;
    int t_wcet;
    int t_activation_num_tot;
    int t_activation_num_curr;
    
    struct rts_params* t_par;
    struct timespec t_activation_time;
        
    t_num = ((struct thread_params*) argv)->num;
    t_par = ((struct thread_params*) argv)->p;
    t_wcet = calc_exec(t_par->budget, t_par->period, t_par->deadline);
    t_period = calc_period(t_par->budget, t_par->period, t_par->deadline);
    t_activation_num_tot = calc_activation_num();
    t_activation_num_curr = 0;
    t_activation_time = get_time_now(rts_get_clock(t_par));
    
    copy_and_signal(&m, &(t_ids[t_num]), gettid());
        
    // thread loop
    while(1) 
    {
        if(computation_ended(&t_activation_num_curr, t_activation_num_tot))
            break;
        
        rts_rsv_begin(t_par);                                   // begin to consume time
        compute_for(&t_activation_time, t_wcet);                // simulate computation
        rts_rsv_end(t_par);                                     // end to consume time
        wait_next_activation(&t_activation_time, t_period);     // sleep until next activation
    }

    return NULL;
}

/**
 * Main process routine.
 * 
 */

int main(int argc, char* argv[]) 
{
    int nthread;                /* number of thread that will be spawned */
    
    rsv_t* rsv_id;              /* identifiers of the reservations, if accepted */
    pthread_t* pt_id;            /* pthread descriptors */ 
    
    struct rts_access rt_chn;        /* two way channel towards daemon */
    struct rts_params* rt_par;       /* parameters for reserving CPU */
    struct thread_params t_par;  /* thread call argument */
    
    if(argc != NPARAMS)
        print_usage(argv[0]);
    
    nthread = atoi(argv[1]);
    
    alloc(nthread, (void**)(&rsv_id), sizeof(rsv_t));
    alloc(nthread, (void**)(&rt_par), sizeof(struct rts_params));
    alloc(nthread, (void**)(&pt_id), sizeof(pthread_t));
    alloc(nthread, (void**)(&t_ids), sizeof(pid_t));
    
    conf_threads(argv[2], nthread, rt_par);
            
    if(rts_daemon_connect(&rt_chn) != RTS_OK)
        exit_err("Unable to connect with the RTS daemon\n");

    if(!rts_cap_query(&rt_chn, RTS_BUDGET))
        exit_err("Notion of budget unsupported!");

    if(!rts_cap_query(&rt_chn, RTS_REMAINING_BUDGET))
        exit_err("Remaining budget retrivial unsupported!\n");
    
    for(int i = 0; i < nthread; i++) 
    {
        if(rts_create_rsv(&rt_chn, &(rt_par[i]), &(rsv_id[i])) != RTS_GUARANTEED)
            exit_err("Can't get scheduling guarantees for thread num: %d!\n", i);

        fill_params(&t_par, i, &(rt_par[i]));
        pthread_create(&(pt_id[i]), NULL, RT_task, (void*)&(t_par));
        lock_and_test(&m, &(t_ids[i]), 0);
        rts_rsv_attach_thread(&rt_chn, &rsv_id, (*t_ids));        
    }
    
    for (int i = 0; i < nthread; i++) 
    {
        // maybe something more.. queries?
        pthread_join(pt_id[i], NULL);
        rts_rsv_destroy(&rt_chn, rsv_id[i]);
    }
    
    rts_daemon_deconnect(&rt_chn);
    
    exit(EXIT_SUCCESS);
}

// MAIN THREAD FUNCTIONS

void print_usage(char* program_name) {
    printf("Usage: %s threadnumber filename", program_name);
    exit(EXIT_FAILURE);
}

void exit_err(char* strerr) {
    printf("FATAL: %s\n", strerr);
    exit(EXIT_FAILURE);
}

pid_t gettid() {
    return syscall(SYS_gettid);
}