#include "../components/pipe.h"
#include "../components/fifo.h"
#include "rt_task.h"

#define FIFO_NAME "prova"

struct rts_msg_req {
    int msg_type;
    struct rt_task t;
    struct pipe p;
};

struct rts_msg_rep {
    int msg_type;
    int response;
};

void rts_req_set_message(struct rts_msg_req* req, int msg_type);

void rts_send_message(struct rts_msg_req* req, struct rts_msg_rep* rep);