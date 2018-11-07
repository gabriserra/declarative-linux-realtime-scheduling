/* 
 * File:   main.h
 * Author: gabrieleserra
 *
 * Created on November 2, 2018, 7:23 PM
 */

#ifndef TEST_H
#define TEST_H

// DATA STRUCT

struct thread_params {
    int num;
    struct rts_params* p;
};

// MAIN THREAD ROUTINES

void print_usage(char* program_name);

void exit_err(char* strerr);

void alloc_data_struct(int nthread, rsv_t* rsv_id, struct rts_params* p, pthread_t* tid);

void skip_comment(FILE* f);

void conf_threads(char* filename, int nthread, struct rts_params* p);

void alloc_tids(int nthread);

void lock_and_test(int* tid);

void wait_and_attach(struct rts_access c, rsv_t* rsv_id, int* tid);

// THREAD ROUTINES

int gettid();

void gettid_and_signal(int num);



#endif /* TEST_H */

