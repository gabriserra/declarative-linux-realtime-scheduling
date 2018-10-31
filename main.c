#include "src/rts_lib.h"
#include <pthread.h>
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>

int computation_ended() {
    return 1;
}

void exit_err(char* strerr) {
    printf("FATAL: %s\n", strerr);
    exit(EXIT_FAILURE);
}

void* RT_task(void* arg) {
    /* parameters for reserving CPU */
    struct rts_params p;

    /* identifier of the reservation, if accepted */
    rsv_t rsv_id;

    /* two way channel towards daemon */
    struct rts_access c;

    if(rts_daemon_connect(&c) != RTS_OK)
        exit_err("Unable to connect with the RTS daemon");

    if(!rts_cap_query(&c, RTS_BUDGET))
        exit_err("Notion of budget unsupported!");

    if(!rts_cap_query(&c, RTS_REMAINING_BUDGET))
        exit_err("Remaining budget retrivial unsupported!");

    rts_params_init(&p);

    /* 40 milliseconds period */
    rts_set_period(&p, 40000);

    /* 25 milliseconds budget */
    rts_set_budget(&p, 25000);

    if(rts_create_rsv(&c, &p, &rsv_id) != RTS_GUARANTEED)
        exit_err("Can't get proper scheduling guarantees!");

    //rts_rsv_attach_thread(&c, rsv_id, gettid());
    rts_rsv_attach_thread(&c, rsv_id, getpid());

    /* params not needed anymore */
    rts_params_cleanup(&p);

    while(!computation_ended()) {
        float rmng_budget;

        rts_rsv_begin(&c, rsv_id);

        compute();
        rts_rsv_get_remaining_budget(&c, rsv_id, &rmng_budget);
        
        if(rmng_budget > 15000)
            return NULL;
            //compute_optional();

        rts_rsv_end(&c, rsv_id);

        wait_next_activation();
    }

    rts_rsv_destroy(&c, rsv_id);

    rts_daemon_deconnect(&c);

    exit(EXIT_SUCCESS);

    return NULL;
}

int main(int argc, char* argv[]) {
    /* thread descriptor */
    pthread_t tid;

    /* spawn thread */
    pthread_create(&tid, NULL, RT_task, NULL);

    /* wait spawned thread */
    pthread_join(tid, NULL);

    exit(EXIT_SUCCESS);
}