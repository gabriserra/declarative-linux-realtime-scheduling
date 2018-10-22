#include "rts_channel.h"
#include <string.h>

int rts_channel_init(struct rts_channel* cc) {
    memset(cc, 0, sizeof(struct rts_channel));
    return unix_socket_init(&(cc->sock), TCP);
}

int rts_channel_connect(struct rts_channel* cc) {
    return unix_socket_connect(&(cc->sock));
}

void rts_channel_recv(struct rts_channel* cc, struct rts_reply* rep) {
    unix_socket_receive(&(cc->sock), (void *)rep, sizeof(struct rts_reply));
}

void rts_channel_send(struct rts_channel* cc, struct rts_request* req) {
    unix_socket_send(&(cc->sock), (void *)req, sizeof(struct rts_request));
}

void rts_channel_d_init(struct rts_channel* cd) {
    memset(cd, 0, sizeof(struct rts_channel));

    if(unix_socket_init(&(cd->sock), TCP) < 0) 
		return -1;

    if(unix_socket_bind(&(cd->sock), CHANNEL_PATH) < 0)
        return -1;

    if(unix_socket_listen(&(cd->sock), BACKLOG_MAX))
        return -1;

    return 0;
}

void rts_channel_d_new_conn(struct rts_channel* cd) {
    unix_socket_check_connection(&(cd->sock));
}

int rts_channel_d_get_size(struct rts_channel* cd) {
    return unix_socket_get_size(&(cd->sock));
}

void rts_channel_d_recv(struct rts_channel* cd, struct rts_request* req, int i) {
    unix_socket_receive(&(cd->sock), i, (void *)req, sizeof(struct rts_request));
}

void rts_channel_d_send(struct rts_channel* cd, struct rts_reply* rep, int i) {
    unix_socket_send(&(cd->sock), i, (void *)rep, sizeof(struct rts_reply));
}