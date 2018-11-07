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

#define EST_NACTIVATION 0 // default 1 (pure number)
#define EST_LASTACTIVATION 1 // (ms from unix epoch)
#define EST_WCET 2  // default 1 (micro)
#define EST_LASTET 3 // micro at lastactivation
#define EST_TOTALET 4 // micro
#define EST_PERIOD 5 // default UINT32_MAX (ms)

#define MICRO_TO_MILLI(var) (var * 1000)

typedef uint32_t rsv_t;

#define stringfy(name) #name

enum SCHED {
	NONE, // nulla
	EDF,
	SSRM,
	DM, // evito
	FP,
	RR,
        CUSTOM, // evito
        NUM_OF_SCHED
};

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

