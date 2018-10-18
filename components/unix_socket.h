/**
 * @file unix_socket.h
 * @author Gabriele Serra
 * @date 13 Oct 2018
 * @brief Contains the interface of a unix socket IPC channel
 *
 * TODO
 */

#ifndef UNIX_SOCKET_H
#define UNIX_SOCKET_H

#include <sys/types.h>
#include <sys/socket.h>
       
// ---------------------------------------------
// DATA STRUCTURES
// ---------------------------------------------

#define TCP SOCK_STREAM
#define UDP SOCK_DGRAM
#define ENABLED 1
#define BACKLOG_MAX 128

struct unix_socket {
    int socket;
    char* filepath;
    fd_set client_set;
}

/** Send a message from a connected socket and return the number of byte sent or -1 otherwise
    Argument: int sock, void* buffer, size_t length
    Return: int */
int s_send(int sock, void* buffer, size_t length);

/** Receive a message from a connected socket and return the number of byte received or -1 otherwise
    for a TCP socket return 0 if client has closed its half connection
    Argument: int sock, void* buffer, size_t length
    Return: int */
int s_recv(int sock, void* buffer, size_t length);

/** Create a server TCP socket and put it awaiting of client connection using library function
    return a socket descriptor in case of success, -1 and set errno otherwise
    Argument: char* addr, ushort port, int max_req
    Return: int */
int s_socket_prep(char* addr, unsigned short port, int max_req);

/** Check errno and print human readable string
    Argument: void
    Return: void */
void print_err();

/** Prepare a fd_set struct to use with select
    Argument: fd_set* des_set
    Return void */
void prepare_set(fd_set* des_set);

/** Add a descriptor to prepared fd_set structure
    Argument: fd_set* des_set, int descriptor
    Return: void */
void add_to_set(fd_set* des_set, int descriptor);

/** Remove a descriptor to fd_set structure
    Argument: fd_set* des_set, int descriptor
    Return: void */
void remove_from_set(fd_set* des_set, int descriptor);

/** Check if a descriptor of fd_set had received some stuff
    Argument: fd_set* des_set, int descriptor
    Return: int */
int is_set(fd_set* des_set, int descriptor);

/** Wait until one or more descriptor had received some stuff
    Argument: fd_set* des_set
    Return: int */
int set_select(fd_set* des_set);

#endif