#include "../components/list_ptr.h"
#include <stdint.h>
#include <time.h>
#include <sys/types.h>

#define RTS_NOT_GUARANTEED 0
#define RTS_GUARANTEED 1

typedef uint32_t rsv_t;

struct rts_params {
    clockid_t 		clk;
    uint32_t		budget;		// worst case ex time [microseconds]
	uint32_t 		period;		// period of task [millisecond]
	uint32_t 		deadline;	// relative deadline [millisecond]
	uint32_t 		priority;	// priority of task [LOW_PRIO, HIGH_PRIO]
};

struct rts_ids { 
    pid_t pid;
    rsv_t rsvid;
};

enum QUERY_TYPE {
    RTS_BUDGET,
    RTS_REMAINING_BUDGET
};

enum REQ_TYPE {
    RTS_CAP_QUERY,
    RTS_RSV_CREATE,
    RTS_RSV_ATTACH,
    RTS_RSV_DETACH,
    RTS_RSV_QUERY,
    RTS_RSV_DESTROY
};

struct rts_request {
    enum REQ_TYPE req_type;
    union {
        struct rts_ids ids;
        struct rts_params param;
        enum QUERY_TYPE query_type;
    } payload;
};

struct rts_request_list {
    struct list_ptr reqs;
};

struct rts_reply {
    int msg_type;
    int response;
};

void rts_request_add(struct rts_request_list* l, struct rts_request* req);

void rts_req_set_message(struct rts_msg_req* req, int msg_type);

void rts_send_message(struct rts_msg_req* req, struct rts_msg_rep* rep);