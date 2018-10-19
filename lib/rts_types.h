#include <stdint.h>
#include <time.h>
#include <sys/types.h>

#define RTS_OK 0
#define RTS_ERROR -1

#define RTS_GUARANTEED 0
#define RTS_NOT_GUARANTEED -1

#define RTS_CONNECTED 0
#define RTS_SOCK_ISSUE -1
#define RTS_DAEMON_UNREACHBLE -2

typedef uint32_t rsv_t;

struct rts_ids { 
    pid_t pid;
    rsv_t rsvid;
};

struct rts_params {
    clockid_t 		clk;
    uint32_t		budget;		// worst case ex time [microseconds]
	uint32_t 		period;		// period of task [millisecond]
	uint32_t 		deadline;	// relative deadline [millisecond]
	uint32_t 		priority;	// priority of task [LOW_PRIO, HIGH_PRIO]
};