/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

/* 
 * File:   memutils.h
 * Author: gabrieleserra
 *
 * Created on November 8, 2018, 11:27 AM
 */

#ifndef MEMUTILS_H
#define MEMUTILS_H

#include <pthread.h>

struct monitor {
    pthread_mutex_t lock;
    pthread_cond_t cond;
};

void alloc(int num, void** pointer, size_t size);

void monitor_init(struct monitor* mem);

void lock_and_test(struct monitor* mem, int* var, int value);

void copy_and_signal(struct monitor* mem, int* var, int value);

#endif /* MEMUTILS_H */

