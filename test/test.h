/* 
 * File:   main.h
 * Author: gabrieleserra
 *
 * Created on November 2, 2018, 7:23 PM
 */

#ifndef TEST_H
#define TEST_H

// params

#define NPARAMS     3       // number of parameters (command line)

// MAIN THREAD ROUTINES

void print_usage(char* program_name);

void exit_err(char* strerr);

// THREAD ROUTINES

int gettid();

#endif /* TEST_H */

