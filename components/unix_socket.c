#include "unix_socket.h"
#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/un.h>

const int REUSE_ADDR = ENABLED;

/** Creates a server IPv4 socket and return a descriptor or -1 otherwise
    Argument: int sock_type
    Return: int */

int unix_socket_init(struct unix_socket* us, int sock_type) {
    int sock;

    memset(us, 0, sizeof(struct unix_socket));
    sock = socket(AF_UNIX, sock_type, 0);

    if(sock < 0)
        return -1;
    
    us->socket = sock;
    return 0;
}

/** Force a server to use port as port number and addr as ip address and return 0 in case of success or -1 otherwise
    Argument: int sock, char* addr, ushort port
    Return: int */
int unix_socket_bind(struct unix_socket* us, char* filepath) {
    struct sockaddr_un server_sockaddr;

    memset(&server_sockaddr, 0, sizeof(struct sockaddr_un));
	server_sockaddr.sun_family = AF_UNIX;
    strcpy(server_sockaddr.sun_path, filepath);
    unlink(filepath);

	return bind(us->socket, (struct sockaddr*)&server_sockaddr, sizeof(struct sockaddr_un));
}

/** Put server awaiting for connection on binded address/port and return 0 in case of success or -1 otherwise
    Argument: int sock, int max_req
    Return: int */
int unix_socket_listen(struct unix_socket* us, int max_req) {
    if(!max_req)
        max_req = BACKLOG_MAX;
    
    return listen(us->socket, max_req);
}

/** Accept a client request of connection, fill sockaddr_in with client information and return a connected_socket descriptor
    in case of success or -1 otherwise
    Argument: int sock, sockaddr_in* client
    Return: int */
int unix_socket_accept(struct unix_socket* us, struct sockaddr_un* client) {
    socklen_t struct_len = sizeof(struct sockaddr_un);
    return accept(us->socket, (struct sockaddr*)client, &struct_len);
}

int unix_socket_connect(struct unix_socket* us, char* filepath) {
    struct sockaddr_un server_sockaddr;

    memset(&server_sockaddr, 0, sizeof(struct sockaddr_un));
	server_sockaddr.sun_family = AF_UNIX;
    strcpy(server_sockaddr.sun_path, filepath);
    return connect(us->socket, (struct sockaddr*)&server_sockaddr, sizeof(struct sockaddr_un));
}

void unix_socket_select(struct unix_socket* us) {
    if(set_select(&(us->client_set)) < 0)
        exit(-1);
}

int s_getpeername(struct unix_socket* us, struct sockaddr_un* client) {
    socklen_t struct_len = sizeof(struct sockaddr_un);
    return getpeername(us->socket, (struct sockaddr *) &client, &struct_len);
}

int s_send(int sock, void* buffer, size_t length) {
    return send(sock, buffer, length, 0);
}

int s_recv(int sock, void* buffer, size_t length) {
    return recv(sock, buffer, length, 0);
}

void print_err() {
    printf("%d: %s\n", errno, strerror(errno));
}


void unix_socket_select(struct unix_socket* us, void* array[], size_t size) {
    int i;
    fd_set temp_client_set;

    temp_client_set = us->client_set;
    
    if(set_select(&temp_client_set) < 0)
        exit(-1);

    for(i = 0; i < FD_SETSIZE; i++) {
        if(is_set(&temp_client_set, i))
            if(i == us->socket)
                add_new_client(us->socket);  
            else
                recv(i, array[i], size, 0);  
    }
    
    return;
}


void unix_socket_prepare_set(struct unix_socket* us) {
    FD_ZERO(&(us->client_set));
}

int unix_socket_add_to_set(struct unix_socket* us) {
    int accept_socket = unix_socket_listen(us, 0);

    if(accept_socket < 0)
        return -1;

    FD_SET(accept_socket, &(us->client_set));
}

void remove_from_set(fd_set* des_set, int descriptor) {
    FD_CLR(descriptor, des_set);
}

int is_set(fd_set* des_set, int descriptor) {
    return FD_ISSET(descriptor, des_set);
}

int set_select(fd_set* des_set) {
    int result = select(FD_SETSIZE, des_set, 0, 0, 0);
    if(result < 0)
        print_err();
    return result;
}
