#include "../components/unix_socket.h"
#include "rts_message.h"
#include "rt_task.h"

#define CHANNEL_PATH "prova"

struct rts_channel_d {
    struct unix_socket sock;
};

struct rts_channel_c {
    struct unix_socket sock;
};

void rts_channel_c_init(struct rts_channel_c* cc);

void rts_channel_d_init(struct rts_channel_d* cd);

void rts_channel_d_newconn(struct rts_channel_d* cd);

void rts_channel_d_recv(struct rts_channel_d* cd, struct rts_request* req, int i);

void rts_channel_d_send(struct rts_channel_d* cd, struct rts_reply* rep, int i);

void rts_channel_c_recv(struct rts_channel_c* cc, struct rts_reply* rep);

void rts_channel_c_send(struct rts_channel_c* cc, struct rts_request* req);

