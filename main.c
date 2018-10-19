#include "src/rts_lib.h"
#include <pthread.h>

int main(int argc, char* argv[]) {
    /* thread descriptor */
    pthread_t tid;

    /* spawn thread */
    pthread_create(&tid, NULL, RT_task, NULL);

    /* wait spawned thread */
    pthread_join(tid, NULL);

    exit(EXIT_SUCCESS);
}

void exit_err(char* strerr) {
    printf("FATAL: %s\n", strerr);
    exit(EXIT_FAILURE);
}

void RT_task(void) {
    /* parameters for reserving CPU */
    rts_params p;

    /* identifier of the reservation, if accepted */
    rts_rsv rsv_id;

    /* two way channel towards daemon */
    rts_channel c;

    if(rts_daemon_connect(c) != RTS_CONNECTED)
        exit_err("")

    if(!rts_cap_query(RTS_CAP_BUDGET))
        exit_err("Notion of budget unsupported!");

    if(!rts_cap_query(RTS_CAP_REMAINING_BUDGET))
        exit_err("Remaining budget retrivial unsupported!");

    rts_params_init(&p);

    /* 40 milliseconds period */
    rts_set_period(&p, 40000);

    /* 25 milliseconds budget */
    rts_set_budget(&p, 25000);

    if(rts_create_rsv(&p, &rsv_id) != RTS_GUARANTEED)
        exit_err("Can't get proper scheduling guarantees!");

    rts_rsv_attach_thread(&p, gettid());

    /* params not needed anymore */
    rts_params_cleanup(&p);

    while(!computation_ended()) {
        int rmng_budget;

        compute();
        rts_rsv_get_remaining_budget(rsv_id, &rmng_budget);
        
        if(rmng_budget > 15000)
            compute_optional();

        wait_next_activation();
    }

    rts_rsv_destroy(rsv_id);
    exit(EXIT_SUCCESS);
}