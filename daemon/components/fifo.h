/**
 * @file fifo.h
 * @author Gabriele Serra
 * @date 13 Oct 2018
 * @brief Contains the interface of a named pipe IPC channel
 *
 * This file contains the interface of a named pipe IPC channel.
 * The named pipe is based on the FIFO mechanism, a file saved
 * in the path provided in the creation phase.
 */

#ifndef FIFO_H
#define FIFO_H

#include <sys/types.h>
#include <sys/stat.h>
       
#define BASE_AUTH 0666

// ---------------------------------------------
// DATA STRUCTURES
// ---------------------------------------------

/**
 * @brief Represent the channel object
 * 
 * The structure channel contains the path/name of the FIFO
 * file, the file descriptor once the channel is open and
 * the auth permission given to the file.
 */
struct fifo {
    char*       name;       /** contains the path/name of the FIFO file */
    int         descriptor; /** contains the file descriptor once open */
    mode_t      auth;       /** contains the auth given to the FIFO file */
};

/**
 * @brief Create the channel given the name and the auth
 * 
 * The function create the channel using the name provided
 * and with the auth passed. If the channel cannot be
 * created, the function print the error and terminate
 * the flow of execution
 * 
 * @param c pointer to the list to be initialized
 * @param name the path and the name of the FIFO file (eg /etc/tmp/file)
 * @param auth permission mask given to the file (eg 0666)
 */
void fifo_create(struct fifo* c, char* name, mode_t auth);

void fifo_open(struct fifo* c, int mode);

void fifo_use(struct fifo* c, char* name);

void fifo_close(struct fifo* c);

void fifo_destroy(struct fifo* c);

int fifo_read(struct fifo* c, void* message, size_t size);

int fifo_write(struct fifo* c, void* message, size_t size);

#endif