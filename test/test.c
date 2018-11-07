#include "test.h"
#include "lib/rts_lib.h"
#include <pthread.h>
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <sys/syscall.h>

#define NPARAMS 3   
#define COLUMN_MAX 80

int* tids;
pthread_mutex_t lock = PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t cond = PTHREAD_COND_INITIALIZER;

void* RT_task(void* argv) {
    struct rts_params* p;
    struct timespec ts;
    
    int num;
    int real_period;
    int real_exec_time;
    int real_activation_num;
        
    num = ((struct thread_params*) argv)->num;
    p = ((struct thread_params*) argv)->p;
    
    gettid_and_signal(num);
    calc_exec_and_period(p->budget, p->period, p->deadline, &real_exec_time, &real_period);
    real_activation_num = rand();
    
    // thread loop

    while(!computation_ended(real_activation_num)) {
        rts_rsv_begin(p);                       // begin to consume time..
        clock_gettime(CLOCK_MONOTONIC, ts);     // get curr time
        compute_for(&ts, real_exec_time);        // simulation compute task
        rts_rsv_end(p);                         // end to consume time
        wait_next_activation(&ts, real_period);
        rts_rsv_renew(p);
    }

    exit(EXIT_SUCCESS);
    return NULL;
}

int main(int argc, char* argv[]) {
    int nthread;                /* number of thread that will be spawned */
    rsv_t* rsv_id;              /* identifiers of the reservations, if accepted */
    struct rts_params* p;       /* parameters for reserving CPU */
    pthread_t* ptid;            /* pthread descriptors */ 
    struct thread_params args;  /* thread call argument */

    /* two way channel towards daemon */
    struct rts_access c;
    
    if(argc != NPARAMS)
        print_usage(argv[0]);
    
    nthread = atoi(argv[1]);
    
    alloc_data_struct(nthread, &rsv_id, &p, &ptid);
    conf_threads(argv[2], nthread, p);
            
    if(rts_daemon_connect(&c) != RTS_OK)
        exit_err("Unable to connect with the RTS daemon\n");

    if(!rts_cap_query(&c, RTS_BUDGET))
        exit_err("Notion of budget unsupported!");

    if(!rts_cap_query(&c, RTS_REMAINING_BUDGET))
        exit_err("Remaining budget retrivial unsupported!\n");
    
    alloc_tids(tids, nthread);
    
    for(int i = 0; i < nthread; i++) {
        /* create the reservation */
        if(rts_create_rsv(&c, &(p[i]), &(rsv_id[i])) != RTS_GUARANTEED)
            exit_err("Can't get scheduling guarantees for thread num: %d!\n", i);
        
        /* fill argument data struct */
        args.num = i;
        args.p = &(p[i]);
        
        /* spawn thread */
        pthread_create(&(ptid[i]), NULL, RT_task, (void*)&(args));
        
        /* wait the tid and attach thread */
        wait_and_attach(&c, &(rsv_id[i]), &(tids[i]));        
    }
    
    for (int i = 0; i < nthread; i++) {
        
        // maybe something more.. queries?
        
        pthread_join(ptid[i], NULL);
        rts_rsv_destroy(&c, rsv_id[i]);
    }
    
    rts_daemon_deconnect(&c);
   
    exit(EXIT_SUCCESS);
}

// MAIN THREAD FUNCTION

void print_usage(char* program_name) {
    printf("Usage: %s threadnumber filename", program_name);
    exit(EXIT_FAILURE);
}

void exit_err(char* strerr) {
    printf("FATAL: %s\n", strerr);
    exit(EXIT_FAILURE);
}

void alloc_data_struct(int nthread, rsv_t** rsv_id, struct rts_params** p, pthread_t** tid) {    
    (*rsv_id) = calloc(nthread, sizeof(rsv_t));
    (*p) = calloc(nthread, sizeof(struct rts_params));
    (*tid) = calloc(nthread, sizeof(pthread_t));

    if((*rsv_id) == NULL || (*p) == NULL || (*tid) == NULL)
        exit_err("Unable to allocate heap memory for threads data!\n");
}

void skip_comment(FILE* f) {
    char buffer[COLUMN_MAX];
    
    while(1) {
        fgets(buffer, COLUMN_MAX, f);
        
        if(buffer[0] != '#')
            break;
    } 
}

void conf_threads(char* filename, int nthread, struct rts_params* p) {
    FILE* f;
    uint64_t priority, budget, deadline, period;
            
    f = fopen(filename, "r");
    
    if(f == NULL)
        exit_err("Error while opening the file.\n");

    skip_comment(f);
    
    for(int i = 0; i < nthread; i++) {
        rts_params_init(&(p[i]));
        
        fscanf(f, "%d", priority);
        if(priority != 0)
            rts_set_priority(&(p[i]), priority);
        
        fscanf(f, "%d", budget);
        if(budget != 0)
            rts_set_budget(&(p[i]), budget);
                
        fscanf(f, "%d", deadline);
        if(deadline != 0)
            rts_set_deadline(&(p[i]), deadline);
                    
        fscanf(f, "%d", period);
        if(period != 0)
            rts_set_period(&(p[i]), period);
    }    
}

void alloc_tids(int* tids, int nthread) {
    tids = calloc(nthread, sizeof(int));
    
    if(tids == NULL)
        exit_err("Unable to allocate heap memory for threads data!\n");
}

void lock_and_test(int* tid) {
    pthread_mutex_lock(&lock);     
    
    if(*tid == 0)        
        pthread_cond_wait(&cond, &lock); 
    
    pthread_mutex_unlock(&lock);   
}

void wait_and_attach(struct rts_access c, rsv_t* rsv_id, int* tid) {
    lock_and_test(tid);
    rts_rsv_attach_thread(&c, &rsv_id, (*tid));
}

// THREAD FUNCTIONS

int gettid() {
    return syscall(SYS_gettid);
}

void gettid_and_signal(int num) {
    pthread_mutex_lock(&lock);
    
    tids[num] = gettid();

    pthread_cond_signal(&cond);
    pthread_mutex_unlock(&lock); 
}

void time_add_us(struct timespec *t, int us) {
    t->tv_sec += us / 1000000;               // convert us to sec and add to sec
    t->tv_nsec += (us % 1000000) * 1000;     // convert and add the remainder to nsec
	
    if (t->tv_nsec > 1000000000) { 
        t->tv_nsec -= 1000000000; 
        t->tv_sec += 1;
    }
}

int time_cmp(struct timespec t1, struct timespec t2) {
    if (t1.tv_sec > t2.tv_sec) 
            return 1; 
    if (t1.tv_sec < t2.tv_sec) 
            return -1;

    if (t1.tv_nsec > t2.tv_nsec) 
            return 1; 
    if (t1.tv_nsec < t2.tv_nsec) 
            return -1; 

    return 0;
}

void calc_exec_and_period(int pbudget, int pperiod, int pdead, int* real_exec_time, int* real_period) {
    if(pbudget != 0)
        *real_exec_time = pbudget;
    else if(pdead != 0)
        *real_exec_time = pdead;
    else if(pperiod != 0)
        *real_exec_time = pperiod;
    else
        *real_exec_time = rand();
        
    if(pperiod != 0)
        *real_period = pperiod;
    else if(pdead != 0)
        *real_period = pdead * 2;
    else if(pbudget != 0)
        *real_period = pbudget * 3;
    else
        *real_period = rand();   
}

void compute_for(int max_micro) {
    int exec_time;
    struct timespec t_init;
    struct timespec t_curr;
    
    if(clock_gettime(CLOCK_MONOTONIC, &t_init) < 0)
        exit_err("Unable to call system clock.\n");
    
    exec_time = rand() % max_micro;

    while(1) {
        clock_gettime(CLOCK_MONOTONIC, &t_curr);
        
        if(time_cmp(t_curr, t_init) > 0)
            break;
    } 
}

int computation_ended(int num_of_activation) {
    static int curr_num_of_activation = 0;
    
    if(num_of_activation < curr_num_of_activation)
        curr_num_of_activation++;
    else
        return 1;
    
    return 0;
}