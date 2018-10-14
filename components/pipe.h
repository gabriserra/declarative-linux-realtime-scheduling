/**
 * @file pipe.h
 * @author Gabriele Serra
 * @date 13 Oct 2018
 * @brief Contains the interface of a unnamed pipe IPC channel
 *
 * This file contains the interface of a named pipe IPC channel.
 * The named pipe is based on the FIFO mechanism, a file saved
 * in the path provided in the creation phase.
 */

#ifndef PIPE_H
#define PIPE_H

#include <sys/types.h>
#include <sys/stat.h>

#define READ_FD 0
#define WRITE_FD 1
       
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
struct pipe {
    int ends[2];
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
int pipe_open(struct pipe* p, int flags);

void pipe_close(struct pipe* p, int end);

int pipe_read(struct pipe* p, void* message, size_t size);

int pipe_write(struct pipe* p, void* message, size_t size);

#endif