

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

#include "../lib/rts_taskset.h"
#include "../lib/rts_channel.h"
#include "../lib/rts_scheduler.h"
#include "../lib/rts_utils.h"
#include <stdlib.h>
#include <stdio.h>
#include <signal.h>

struct rts_deamon {
    struct rts_channel chann;
    struct rts_scheduler sched;
    struct rts_taskset tasks;
} data;

void daemon_term() {
    printf("Killed..\n");
}

void daemon_init() {
    rts_channel_d_init(&(data.chann));
    rts_taskset_init(&(data.tasks));
    rts_scheduler_init(&(data.sched), read_rt_kernel_budget());
}

void daemon_loop() {
    int i;
    struct rts_request req;
    struct rts_reply rep;

    while(1) {

        if(rts_channel_d_select(&(data.chann)) < 0)
            exit(EXIT_FAILURE);

        for(i = 0; i < rts_channel_d_get_size(&(data.chann)); i++) {
            if(rts_channel_d_has_data(&(data.chann), i))
                if(rts_channel_d_new_conn(&(data.chann))) // miss i
                    rts_channel_d_add_conn(&(data.chann), i);
                else
                    // receive and reply
        }

    }

    return;
}

int main(int argc, char* argv[]) {
    printf("rtsdaemond - Daemon started.\n");
    
    signal(SIGINT, daemon_term);
    daemon_init();
    daemon_loop();
}





