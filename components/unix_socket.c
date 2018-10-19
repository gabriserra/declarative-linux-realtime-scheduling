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

static int unix_socket_init(struct unix_socket* us, int sock_type) {
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
static int unix_socket_bind(struct unix_socket* us, char* filepath) {
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
static int unix_socket_listen(struct unix_socket* us, int max_req) {
    if(!max_req)
        max_req = BACKLOG_MAX;
    
    return listen(us->socket, max_req);
}

/** Accept a client request of connection, fill sockaddr_in with client information and return a connected_socket descriptor
    in case of success or -1 otherwise
    Argument: int sock, sockaddr_in* client
    Return: int */
static int unix_socket_accept(struct unix_socket* us, struct sockaddr_un* client) {
    socklen_t struct_len = sizeof(struct sockaddr_un);
    return accept(us->socket, (struct sockaddr*)client, &struct_len);
}

static int s_getpeername(struct unix_socket* us, struct sockaddr_un* client) {
    socklen_t struct_len = sizeof(struct sockaddr_un);
    return getpeername(us->socket, (struct sockaddr *) &client, &struct_len);
}

int s_send(int sock, void* buffer, size_t length) {
    return send(sock, buffer, length, 0);
}

int s_recv(int sock, void* buffer, size_t length) {
    return recv(sock, buffer, length, 0);
}

int unix_socket_server_tcp(struct unix_socket* us, char* filename, int max_req) {	
    if(unix_socket_init(us, TCP) < 0) 
		return -1;
    if(unix_socket_bind(us, filename) < 0)
        return -1;
    if(unix_socket_listen(us, max_req))
        return -1;

    return 0;
}

void print_err() {
    printf("%d: %s\n", errno, strerror(errno));
}

void server_function(int main_sock) {
    fd_set temp_client_set;
    int current_sock;
    
    //Block until input arrives on one or more active sockets
    temp_client_set = active_client_set;
    
    if(set_select(&temp_client_set) < 0)
        exit(-1);

    //Service all the sockets with input pending.
    for(current_sock = 0; current_sock < FD_SETSIZE; current_sock++) {
        if(is_set(&temp_client_set, current_sock)) {
            if(current_sock == main_sock)   // Connection request on original socket.    
                add_new_client(main_sock);  
            else
                if(!receive_and_reply(current_sock)) // Data arriving on an already-connected socket.
                    handle_lose_connection(current_sock);
        }  
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