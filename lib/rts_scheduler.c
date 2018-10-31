#include "rts_scheduler.h"

void rts_scheduler_init(struct rts_scheduler* s, float sysbudget) {
    s->sysbudget = sysbudget;
    rts_taskset_init(&(s->taskset));

    // leggo un file e
}

void rts_scheduler_delete(struct rts_scheduler* s, pid_t pid) {
    struct node_ptr* iterator;
    struct rts_task* task;
    
    iterator = rts_taskset_get_i_node(&(s->taskset), 0);

    for(iterator; iterator != NULL; iterator = iterator->next) {
        task = (struct rts_task*) iterator->elem;
        if(task->ptid == pid)
            rts_taskset_remove(&(s->taskset), pid);
    }        
}

float rts_scheduler_get_remaining_budget(struct rts_scheduler* s) {

}

rsv_t rts_scheduler_create(struct rts_scheduler* s, struct rts_params* tp, int cli_id) {

}

int rts_scheduler_attach(struct rts_scheduler* s, int cli_id, rsv_t rsvid, pid_t pid) {

}

int rts_scheduler_detach(struct rts_scheduler* s, int cli_id, rsv_t rsvid, pid_t pid) {

}

float rts_scheduler_get_task_budget(struct rts_scheduler* s, rsv_t rsvid) {

}

float rts_scheduler_get_task_rem_budget(struct rts_scheduler* s, rsv_t rsvid) {

}

int rts_scheduler_destroy_rsv(struct rts_scheduler* s, rsv_t rsvid) {

}