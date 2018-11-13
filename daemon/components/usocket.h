/**
 * @file usocket.h
 * @author Gabriele Serra
 * @date 13 Oct 2018
 * @brief Contains the interface of a unix socket IPC channel
 *
 * TODO
 */

#ifndef USOCKET_H
#define USOCKET_H

#include <sys/types.h>
#include <sys/socket.h>
#include <sys/select.h>
       
// ---------------------------------------------
// DATA STRUCTURES
// ---------------------------------------------

#define TCP SOCK_STREAM
#define UDP SOCK_DGRAM

#define SELECT_MAX_WAIT 50
#define BACKLOG_MAX 128
#define SET_MAX_SIZE FD_SETSIZE

struct usocket {
    int socket;
    char* filepath;
    int conn_set_max;
    fd_set conn_set;
};

// 
// COMMON FOR CLIENT / SERVER
//

int usocket_init(struct usocket* us, int socktype);

int usocket_bind(struct usocket* us, char* filepath);

// 
// SPECIFIC FOR CLIENTS
//

int usocket_connect(struct usocket* us, char* filepath);

int usocket_recv(struct usocket* us, void* elem, size_t size);

int usocket_send(struct usocket* us, void* elem, size_t size);

// 
// SPECIFIC FOR SERVERS
//

int usocket_listen(struct usocket* us, int max_req);

int usocket_accept(struct usocket* us);

int usocket_getpeername(struct usocket* us, char** name, size_t* size, int fd);

int usocket_recvfrom(struct usocket* us, void* elem, size_t size, int i);

int usocket_sendto(struct usocket* us, void* elem, size_t size, int i);

int usocket_recvall(struct usocket* us, void* data, int nrecv[SET_MAX_SIZE], size_t size);

int usocket_add_connections(struct usocket* us);

void usocket_remove_connections(struct usocket* us, int fd);

int usocket_nonblock(struct usocket* us);

int usocket_block(struct usocket* us);

int usocket_timeout(struct usocket* us, int ms);

int usocket_get_maxfd(struct usocket* us);

void usocket_prepare_recv(struct usocket* us);

#endif
