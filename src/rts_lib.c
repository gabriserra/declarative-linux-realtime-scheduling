#include "rts_lib.h"
#include <unistd.h>
#include <string.h>

int rts_daemon_connect(struct rts_access* c) {
    if(rts_access_init(c) < 0)
        return RTS_ERROR;
        
    if(rts_access_connect(c) < 0)
        return RTS_ERROR;

    c->req.req_type = RTS_CONNECTION;
    c->req.payload.ids.pid = getpid();

    if(rts_access_send(c) < 0)
        return RTS_ERROR;
    if(rts_access_recv(c) < 0)
        return RTS_ERROR;

    if(c->rep.rep_type == RTS_CONNECTION_ERR)
        return RTS_ERROR;

    return RTS_OK;
}

float rts_cap_query(struct rts_access* c, enum QUERY_TYPE type) {
    c->req.req_type = RTS_CAP_QUERY;
    c->req.payload.query_type = type;

    if(rts_access_send(c) < 0)
        return RTS_ERROR;
    if(rts_access_recv(c) < 0)
        return RTS_ERROR;

    if(c->rep.rep_type == RTS_CAP_QUERY_ERR)
        return RTS_ERROR;
    
    return c->rep.payload; 
}

void rts_params_init(struct rts_params *tp) {
    memset(tp, 0, sizeof(struct rts_params));
}

void rts_set_period(struct rts_params* tp, uint32_t period) {
    tp->period = period;
}

int rts_get_period(struct rts_params* tp) {
    return tp->period;
}

void rts_set_budget(struct rts_params* tp, uint32_t budget) {
    tp->budget = budget;
}

int rts_get_budget(struct rts_params* tp) {
    return tp->budget;
}

void rts_set_deadline(struct rts_params* tp, uint32_t deadline) {
    tp->deadline = deadline;
}

int rts_get_deadline(struct rts_params* tp) {
    return tp->deadline;
}

void rts_set_priority(struct rts_params* tp, uint32_t priority) {
    tp->priority = priority;
}

void rts_params_cleanup(struct rts_params* tp) {
    rts_params_init(tp);
}

int rts_create_rsv(struct rts_access* c, struct rts_params* tp, rsv_t* id) {
    c->req.req_type = RTS_RSV_CREATE;
    memcpy(&(c->req.payload.param), tp, sizeof(struct rts_params));
    
    if(rts_access_send(c) < 0)
        return RTS_ERROR;
    if(rts_access_recv(c) < 0)
        return RTS_ERROR;

    if(c->rep.rep_type == RTS_RSV_CREATE_UN || c->rep.rep_type == RTS_RSV_CREATE_ERR)
        return RTS_NOT_GUARANTEED;

    *id = (rsv_t) c->rep.payload;
    return RTS_GUARANTEED;
}

// TO BE DONE
int rts_rsv_begin(struct rts_access* c, rsv_t* id) {
    return 1;
}

// to be done
int rts_rsv_end(struct rts_access* c, rsv_t* id) {
    return 1;
}

int rts_rsv_attach_thread(struct rts_access* c, rsv_t id, pid_t pid) {
    c->req.req_type = RTS_RSV_ATTACH;
    c->req.payload.ids.rsvid = id;
    c->req.payload.ids.pid = pid;

    if(rts_access_send(c) < 0)
        return RTS_ERROR;
    if(rts_access_recv(c) < 0)
        return RTS_ERROR;

    if(c->rep.rep_type == RTS_RSV_ATTACH_ERR)
        return RTS_ERROR;

    return RTS_OK;
}

int rts_rsv_detach_thread(struct rts_access* c, rsv_t id) {
    c->req.req_type = RTS_RSV_DETACH;
    c->req.payload.ids.rsvid = id;

    if(rts_access_send(c) < 0)
        return RTS_ERROR;
    if(rts_access_recv(c) < 0)
        return RTS_ERROR;

    if(c->rep.rep_type == RTS_RSV_DETACH_ERR)
        return RTS_ERROR;

    return RTS_OK;
}

int rts_rsv_get_remaining_budget(struct rts_access* c, rsv_t id, float* budget) {
    c->req.req_type = RTS_RSV_QUERY;
    c->req.payload.ids.rsvid = id;    

    if(rts_access_send(c) < 0)
        return RTS_ERROR;
    if(rts_access_recv(c) < 0)
        return RTS_ERROR;

    if(c->rep.rep_type == RTS_RSV_QUERY_ERR)
        return RTS_ERROR;

    *budget = c->rep.payload;
    return RTS_OK;
}

int rts_rsv_destroy(struct rts_access* c, rsv_t id) {
    c->req.req_type = RTS_RSV_DESTROY;
    c->req.payload.ids.rsvid = id;

    if(rts_access_send(c) < 0)
        return RTS_ERROR;
    if(rts_access_recv(c) < 0)
        return RTS_ERROR;

    if(c->rep.rep_type == RTS_RSV_DESTROY_ERR)
        return RTS_ERROR;

    return RTS_OK;
}

int rts_daemon_deconnect(struct rts_access* c) {
    c->req.req_type = RTS_DECONNECTION;
    c->req.payload.ids.pid = getpid();

    if(rts_access_send(c) < 0)
        return RTS_ERROR;
    if(rts_access_recv(c) < 0)
        return RTS_ERROR;

    if(c->rep.rep_type == RTS_DECONNECTION_ERR)
        return RTS_ERROR;

    return RTS_OK;
}

// -----------------------------
// utility for scheduling
//

// ---
// Copies a source time variable ts in a destination variable pointed by td
// timespec* td: pointer to destination timespec data structure
// timespec ts: source timespec data structure
// return: void
// ---
static void time_copy(struct timespec* td, struct timespec ts) {
    td->tv_sec  = ts.tv_sec;
    td->tv_nsec = ts.tv_nsec;
}

// ---
// Adds a value ms expressed in milliseconds to the time variable pointed by t
// timespec* t: pointer to timespec data structure
// int ms: value in milliseconds to add to t
// return: void
// ---
static void time_add_ms(struct timespec *t, int ms) {
    t->tv_sec += ms/1000;            // convert ms to sec and add to sec
    t->tv_nsec += (ms%1000)*1000000; // convert and add the remainder to nsec
	
    // if nsec is greater than 10^9 means has reached 1 sec
    if (t->tv_nsec > 1000000000) { 
            t->tv_nsec -= 1000000000; 
            t->tv_sec += 1;
    }
}

// ---
// Compares time var t1, t2 and returns 0 if are equal, 1 if t1>t2, ‐1 if t1<t2
// timespec t1: first timespec data structure
// timespec t2: second timespec data structure
// return: int - 1 if t1 > t2, -1 if t1 < t2, 0 if they are equal
// ---
static int time_cmp(struct timespec t1, struct timespec t2) {
	// at first sec value is compared
	if (t1.tv_sec > t2.tv_sec) 
		return 1; 
	if (t1.tv_sec < t2.tv_sec) 
		return -1;
	//  at second nano sec value is compared
	if (t1.tv_nsec > t2.tv_nsec) 
		return 1; 
	if (t1.tv_nsec < t2.tv_nsec) 
		return -1; 
	return 0;
}

void compute_for(int exec_ms) {
    struct timespec end;
    struct timespec curr;
    
    // estimate the end of computation
    clock_gettime(CLOCK_REALTIME, &end);
    time_add_ms(&end, exec_ms);
                
    printf("Begin..\n");
    
    while(1) {
        clock_gettime(CLOCK_REALTIME, &curr);
        
        if(time_cmp(&curr, &end) >= 0)
            break;
    }    
    
    printf("The end.\n");
}

// ---
// Reads the curr time and computes the next activ time and the deadline
// task_par* tp: pointer to tp data structure of the thread
// return: void
// ---
void set_period(struct timespec* t, int period) {	
	// get current clock value
	clock_gettime(CLOCK_MONOTONIC, t); 

	// adds period
	time_add_ms(t, period); 
}

void wait_next_activation(struct timespec* t, int period) {
    clock_nanosleep(CLOCK_MONOTONIC, TIMER_ABSTIME, t, NULL);
    time_add_ms(&t, period);
}
