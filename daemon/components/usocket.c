#include "usocket.h"
#include <stdlib.h>
#include <string.h>
#include <sys/un.h>
#include <fcntl.h>
#include <unistd.h>

/** Creates a server IPv4 socket and return a descriptor or -1 otherwise
    Argument: int sock_type
    Return: int */
int usocket_init(struct usocket* us, int socktype) {
    int sock;

    memset(us, 0, sizeof(struct usocket));
    sock = socket(AF_UNIX, socktype, 0);

    if(sock < 0)
        return -1;
    
    us->socket = sock;
    us->conn_set_max = 0;
    us->filepath = NULL;
    FD_ZERO(&(us->conn_set));
    return 0;
}

/** Force a server to use port as port number and addr as ip address and return 0 in case of success or -1 otherwise
    Argument: int sock, char* addr, ushort port
    Return: int */
int usocket_bind(struct usocket* us, char* filepath) {
    struct sockaddr_un usock_sockaddr;

    memset(&usock_sockaddr, 0, sizeof(struct sockaddr_un));
	usock_sockaddr.sun_family = AF_UNIX;
    strcpy(usock_sockaddr.sun_path, filepath);
    unlink(filepath);

	return bind(us->socket, (struct sockaddr*)&usock_sockaddr, sizeof(struct sockaddr_un));
}

int usocket_connect(struct usocket* us, char* filepath) {
    struct sockaddr_un usock_sockaddr;

    memset(&usock_sockaddr, 0, sizeof(struct sockaddr_un));
	usock_sockaddr.sun_family = AF_UNIX;
    strcpy(usock_sockaddr.sun_path, filepath);
    return connect(us->socket, (struct sockaddr*)&usock_sockaddr, sizeof(struct sockaddr_un));
}

int usocket_recv(struct usocket* us, void* elem, size_t size) {
    return recv(us->socket, elem, size, 0);
}

int usocket_send(struct usocket* us, void* elem, size_t size) {
    return send(us->socket, elem, size, 0);
}

/** Put server awaiting for connection on binded address/port and return 0 in case of success or -1 otherwise
    Argument: int sock, int max_req
    Return: int */
int usocket_listen(struct usocket* us, int max_req) {
    if(!max_req)
        max_req = BACKLOG_MAX;
    
    return listen(us->socket, max_req);
}

/** Accept a client request of connection, fill sockaddr_in with client information and return a connected_socket descriptor
    in case of success or -1 otherwise
    Argument: int sock, sockaddr_in* client
    Return: int */
int usocket_accept(struct usocket* us) {
    struct sockaddr_un client;
    socklen_t struct_len;
    
    struct_len = sizeof(struct sockaddr_un);
    return accept(us->socket, (struct sockaddr*)&client, &struct_len);
}

int usocket_getpeername(struct usocket* us, char** name, size_t* size, int fd) {
    struct sockaddr_un peer;
    socklen_t struct_len;
    
    name = NULL;
    size = NULL;
    struct_len = sizeof(struct sockaddr_un);

    if(getpeername(fd, (struct sockaddr *) &peer, &struct_len) < 0)
        return -1;

    *size = strlen(peer.sun_path);
    *name = malloc(*size);
    strncpy(*name, peer.sun_path, *size);

    return 0;
}

int usocket_recvfrom(struct usocket* us, void* elem, size_t size, int i) {
    return recv(i, elem, size, 0);
}

int usocket_sendto(struct usocket* us, void* elem, size_t size, int i) {
    return send(i, elem, size, 0);
}

int usocket_nonblock(struct usocket* us) {
    return fcntl(us->socket, F_SETFL, O_NONBLOCK);
}

int usocket_block(struct usocket* us) {
    return fcntl(us->socket, F_SETFL, ~O_NONBLOCK);
}

int usocket_timeout(struct usocket* us, int ms) {
    struct timeval tv;
    
    tv.tv_sec = 0;
    tv.tv_usec = ms * 1000;
    
    if(setsockopt(us->socket, SOL_SOCKET, SO_SNDTIMEO, (const char*)&tv, sizeof tv) < 0)
        return -1;
    if(setsockopt(us->socket, SOL_SOCKET, SO_RCVTIMEO, (const char*)&tv, sizeof tv) < 0)
        return -1;

    return 0;
}

int usocket_get_maxfd(struct usocket* us) {
    return us->conn_set_max;
}

void usocket_prepare_recv(struct usocket* us) {
    FD_ZERO(&(us->conn_set));
    FD_SET(us->socket, &(us->conn_set));
    us->conn_set_max = us->socket;
}

int usocket_recvall(struct usocket* us, void* data, int nrecv[SET_MAX_SIZE], size_t size) {
    int i;
    fd_set temp_conn_set;

    FD_ZERO(&temp_conn_set);
    FD_COPY(&(us->conn_set), &temp_conn_set);
    
    if(select(us->conn_set_max+1, &temp_conn_set, 0, 0, 0) < 0)
        return -1;

    for(i = 0; i <= us->conn_set_max; i++) {
        if(!FD_ISSET(i, &temp_conn_set))
            continue;

        if(i == us->socket) {
            nrecv[i] = usocket_add_connections(us);
            continue;
        }
        
        nrecv[i] = recv(i, data+(i*size), size, 0);

        if(!nrecv[i])
            FD_CLR(i, &(us->conn_set));
    }

    return 0;
}

int usocket_add_connections(struct usocket* us) {
    int newfd = usocket_accept(us);

    if(newfd < 0)
        return -1;

    FD_SET(newfd, &(us->conn_set));
    us->conn_set_max = us->conn_set_max > newfd ? us->conn_set_max : newfd;

    return newfd;
}

void usocket_remove_connections(struct usocket* us, int fd) {
    FD_CLR(fd, &(us->conn_set));
}
