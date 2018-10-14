#define _GNU_SOURCE

#include "rts_channel.h"
#include <fcntl.h> 

void rts_send_message(struct rts_msg_req* req, struct rts_msg_rep* rep) {
    struct fifo f;

    pipe_open(&(req->p), O_DIRECT);
    
    fifo_use(&f, FIFO_NAME);
    fifo_open(&f, O_WRONLY);

    fifo_write(&f, req, sizeof(struct rts_msg_req));
    pipe_close(&(req->p), WRITE_FD);

    pipe_read(&(req->p), &rep, sizeof(struct rts_msg_rep));
}

void rts_req_set_message(struct rts_msg_req* req, int msg_type) {
    req->msg_type = msg_type;
}