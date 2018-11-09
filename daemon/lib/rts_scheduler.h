/*

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

--> PER FORZA, SE NON SPECIFICHI NE WCET NE DEADLINE
    DEVI SPECIFICARE UNA AVG, ALTRIMENTI NON ACCETTO LO SCHEDULING
--> TASK
    aggiorno media esecuzione
--> TEST:
    -calcolo budget richiesto dalla prenotazione
    -controllo che sys_budget passato abbia almeno tot budget rimasto
    -specifico il numero di parametri ideali e quelli che mi hanno passato
    -calcolo indice schedulabilità
--> SCELGO QUELLO CON INDICE PIU ALTO.
    A PARITA DI INDICE QUELLO CON KERNEL PRIO PIU ALTA
--> SCHEDULO

--> SCHED_EDF SONO BASATI SU SCHED_DEAD -> non devo fare nulla solo test amm
        -> SCHED_EDF(deadline/periodo/wcet)
--> SCHED_RM / SCHED_DM / SCHED_FP -> BASATI SU SCHED_FIFO -> devo allocare priorità
        -> SCHED_DM (deadline/periodo diversi fra di loro)
        -> SCHED_RM/SS (wcet/periodo)
        -> SCHED_FP (prio)
--> SCHED_RTS -> BASATO SU SCHED_RR -> solo test amm (avg) | 

--> 25 prio per SCHED_RM/DM/FP? 98 prio per 3 sched -> 32 per ALGO

*/

#include "rts_types.h"
#include "rts_taskset.h"
#include "rts_plugin.h"
#include <sys/types.h>

struct rts_scheduler {
    float sys_rt_budget;  // max 1.0 - min 0.0
    struct rts_taskset taskset;
    int num_of_plugin;
    struct rts_sched_plugin* plg;
};

void rts_scheduler_init(struct rts_scheduler* s, float sys_free_budget);

void rts_scheduler_delete(struct rts_scheduler* s, pid_t pid);

float rts_scheduler_get_remaining_budget(struct rts_scheduler* s);

rsv_t rts_scheduler_create(struct rts_scheduler* s, struct rts_params* tp);

int rts_scheduler_attach(struct rts_scheduler* s, rsv_t rsvid, pid_t pid);

int rts_scheduler_detach(struct rts_scheduler* s, rsv_t rsvid);

float rts_scheduler_get_task_budget(struct rts_scheduler* s, rsv_t rsvid);

float rts_scheduler_get_task_rem_budget(struct rts_scheduler* s, rsv_t rsvid);

int rts_scheduler_destroy_rsv(struct rts_scheduler* s, rsv_t rsvid);