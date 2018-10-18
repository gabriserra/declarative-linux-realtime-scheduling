

/*

start del demone
creo server AF_UNIX
check plugin disponibili? -> dentro scheduler

loop attendo (sul file? o aspetto signal?)

creo task con caratteristiche richieste
aggiungo task al taskset corrente

leggo max fattore utilizzazione su proc

- SE CHIESTO ESPLICITAMENTE USARE UN PLUGIN?
- SE NESSUN TASK SPECIFICA PRIO/DEADLINE/PERIODO -> SCHED RR
- SE ALMENO UNO SPECIFICA PRIORITA -> SCHED FIFO
- SE ALMENO UNO SPECIFICA PRIO/PERIODO -> RM o EDF (OVER SCHED_FIFO)
- SE ALMENO UNO SPECIFICA PRIO/DEADLINE = PERIODO -> RM o EDF (OVER SCHED_FIFO)
- SE ALMENO UNO SPECIFICA PRIO/DEADLINE != PERIODO -> DM o EDF (OVER SCHED_FIFO)
- SE ALMENO UNO SPECIFICA ISOLAMENTO TEMPORALE -> SPORADIC (OVER SCHED_DEADLINE)

- per ogni item, se supero fattore U max, rigetto la richiesta
- per ogni item, se non supero fattore U max ma il test di schedulabilità fallisce
  provo a passare ad EDF (se non voglio isolamento temporale)

*/

// TODO -> sostituire tutte le printf con syslog

#include "lib/rt_taskset.h"
#include "lib/rts_channel.h"
#include <stdlib.h>
#include <stdio.h>

#define PROC_RT_PERIOD_FILE "/proc/sys/kernel/sched_rt_period_us"
#define PROC_RT_RUNTIME_FILE "/proc/sys/kernel/sched_rt_runtime_us"

struct rts_deamon {
    struct rts_channel chann;
    struct rts_scheduler sched;
    struct rts_taskset tasks;
    struct rts_request reqs;
    struct rts_reply reps;
} data;

int main(int argc, char* argv[]) {
    printf("rtsdaemond - Daemon started.\n");
    
    signal(SIGINT, daemon_term);
    daemon_init();
    daemon_loop();
}

void daemon_init() {
    channel_init_server(&(data->chann));
    taskset_init(&(data->tasks));
    scheduler_init(&(data->sched), read_rt_kernel_budget());
    request_init(&(data->reqs));
}

void daemon_loop() {
    while(1) {
        channel_check_new_conn(&(data->chann));

        //for 1 to size
            channel_receive_req(&(data->chann), &(data->reqs), i);
            //daemon_handle_req(&(data->reqs), &(data->reps));
            channel_send_rep(&(data->chann), &(data->reps), i);
    }
}

void daemon_term() {
    printf("Killed..\n");
}

