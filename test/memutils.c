/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

#include "memutils.h"
#include <stdlib.h>
#include <stdio.h>

static void exit_err(char* strerr) {
    printf("FATAL: %s\n", strerr);
    exit(EXIT_FAILURE);
}

void alloc(int num, void** pointer, size_t size) {
    (*pointer) = calloc(num, size);
    
    if((*pointer) == NULL)
        exit_err("Unable to allocate heap memory for threads data!\n");
}

void monitor_init(struct monitor* m) {
    pthread_mutex_init(&(m->lock), NULL);
    pthread_cond_init(&(m->cond), NULL);
}

void lock_and_test(struct monitor* m, int* var, int value) {
    pthread_mutex_lock(&(m->lock));     
    
    if((*var) == value)        
        pthread_cond_wait(&(m->cond), &(m->lock)); 
    
    pthread_mutex_unlock(&(m->lock));   
}

void copy_and_signal(struct monitor* m, int* var, int value) {
    pthread_mutex_lock(&(m->lock));
    
    *var = value;

    pthread_cond_signal(&(m->cond));
    pthread_mutex_unlock(&(m->lock)); 
}