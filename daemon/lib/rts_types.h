#ifndef RTS_TYPES_H
#define RTS_TYPES_H

#include <stdint.h>
#include <time.h>
#include <sys/types.h>
#include "../components/shatomic.h"

#define RTS_OK 0
#define RTS_ERROR -1

#define RTS_GUARANTEED 0
#define RTS_NOT_GUARANTEED -1

// estimated parameters

#define EST_NUM_ACTIVATION  0   // default: 0 [pure number]
#define EST_ABS_ACTIVATION  1   // default: time at first activation [ms] 
#define EST_ABS_FINISHING   2   // default: time at first finishing time [ms]
#define EST_PERIOD          3   // default: MAX_INT_32 [ms]
#define EST_WCET            4   // default: MAX_INT_32 / 3 [ms]
#define EST_PERTHREADCLK    5   // default: 0

typedef uint32_t rsv_t;

enum QUERY_TYPE {
    RTS_BUDGET,
    RTS_REMAINING_BUDGET
};

enum REQ_TYPE {
    RTS_CONNECTION,
    RTS_CAP_QUERY,
    RTS_RSV_CREATE,
    RTS_RSV_ATTACH,
    RTS_RSV_DETACH,
    RTS_RSV_QUERY,
    RTS_RSV_DESTROY,
    RTS_DECONNECTION
};

enum REP_TYPE {
    RTS_REQUEST_ERR,
    RTS_CONNECTION_OK,
    RTS_CONNECTION_ERR,
    RTS_CAP_QUERY_OK,
    RTS_CAP_QUERY_ERR,
    RTS_RSV_CREATE_OK,
    RTS_RSV_CREATE_UN,
    RTS_RSV_CREATE_ERR,
    RTS_RSV_ATTACH_OK,
    RTS_RSV_ATTACH_ERR,
    RTS_RSV_DETACH_OK,
    RTS_RSV_DETACH_ERR,
    RTS_RSV_QUERY_OK,
    RTS_RSV_QUERY_ERR,
    RTS_RSV_DESTROY_OK,
    RTS_RSV_DESTROY_ERR,
    RTS_DECONNECTION_OK,
    RTS_DECONNECTION_ERR
};

enum CLIENT_STATE {
    EMPTY,
    CONNECTED,
    DISCONNECTED,
    ERROR
};

struct rts_params {
    clockid_t 		clk;
    uint32_t		budget;		// worst case ex time [microseconds]
    uint32_t 		period;		// period of task [millisecond]
    uint32_t 		deadline;	// relative deadline [millisecond]
    uint32_t 		priority;	// priority of task [LOW_PRIO, HIGH_PRIO]
    struct shatomic     estimatedp;     // nactivation, period, wcet
};

struct rts_ids {
    pid_t pid;
    rsv_t rsvid;
};

struct rts_request {
    enum REQ_TYPE req_type;
    union {
        struct rts_ids ids;
        struct rts_params param;
        enum QUERY_TYPE query_type;
    } payload;
};

struct rts_reply {
    enum REP_TYPE rep_type;
    float payload;
};

struct rts_client {
    enum CLIENT_STATE state;
    pid_t pid;
};

#endif	// RTS_TYPES_H

