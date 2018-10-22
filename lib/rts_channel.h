#include "rts_message.h"
#include "../components/unix_socket.h"

#define CHANNEL_PATH "prova"

struct rts_channel {
    struct unix_socket sock;
};

int rts_channel_init(struct rts_channel* cc);

int rts_channel_connect(struct rts_channel* cc);

void rts_channel_recv(struct rts_channel* cc, struct rts_reply* rep);

void rts_channel_send(struct rts_channel* cc, struct rts_request* req);

void rts_channel_d_init(struct rts_channel* cd);

void rts_channel_d_new_conn(struct rts_channel* cd);

int rts_channel_d_get_size(struct rts_channel* cd);

void rts_channel_d_recv(struct rts_channel* cd, struct rts_request* req, int i);

void rts_channel_d_send(struct rts_channel* cd, struct rts_reply* rep, int i);

