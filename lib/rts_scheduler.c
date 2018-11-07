#include "rts_scheduler.h"

void rts_scheduler_init(struct rts_scheduler* s, float sys_rt_budget) {
    s->sys_rt_budget = sys_rt_budget;
    rts_taskset_init(&(s->taskset));
    rts_plugins_init(s->plg, s->num_of_plugin);
}

void rts_scheduler_delete(struct rts_scheduler* s, pid_t ppid) {   
    struct rts_task* t;
    
    while(1) {
        t = rts_taskset_remove(&(s->taskset), ppid); // to be implemented
        
        if(t == NULL)
            break;
        
        rts_scheduler_mem_detach(&(t->est_param))
        rts_task_destroy(t);
    }
   
    for(int i = 0; i < s->num_of_plugin; i++)
        s->plg[i].calc_budget(&(s->taskset));
}

float rts_scheduler_get_remaining_budget(struct rts_scheduler* s) {
    float free_budget = s->sys_rt_budget;
    
    for(int i = 0; i < s->num_of_plugin; i++)
        free_budget -= s->plg[i].used_budget;
    
    return free_budget;
}

rsv_t rts_scheduler_create(struct rts_scheduler* s, struct rts_params* tp) {
    static rsv_t rsv_id = 0;
    struct rts_task* t;
    
    rts_task_init(t, 0, tp->clk);
    t->id = rsv_id;
    t->period = tp->period;
    t->wcet = tp->budget;
    t->deadline = tp->deadline;
    t->priority = tp->priority;
    
    if(rts_scheduler_mem_attach(t->est_param) < 0)
        return -1;
    if(rts_scheduler_assign(s, t) < 0)
        return -1;
        
    return rsv_id;
}

int rts_scheduler_attach(struct rts_scheduler* s, rsv_t rsvid, pid_t pid) {
    struct rts_task* t;
    struct node_ptr* iterator = s->taskset->tasks->root;
    
    for(iterator; iterator != NULL; iterator = iterator->next) {
        t = (struct rts_task)iterator->elem;
        
        if(t->id == rsvid) {
            t->tid = pid;
            return rts_scheduler_schedule(s, t);
        }
    } 
    
    return -1;
}

int rts_scheduler_detach(struct rts_scheduler* s, rsv_t rsvid) {
    struct rts_task* t;
    struct node_ptr* iterator = s->taskset->tasks->root;
    
    for(iterator; iterator != NULL; iterator = iterator->next) {
        t = (struct rts_task)iterator->elem;
        
        if(t->id == rsvid)
            return rts_scheduler_deschedule(s, t);
    } 
    
    return -1;
}

float rts_scheduler_get_task_budget(struct rts_scheduler* s, rsv_t rsvid) {
    struct rts_task* t;
    struct node_ptr* iterator = s->taskset->tasks->root;

    for(iterator; iterator != NULL; iterator = iterator->next) {
        t = (struct rts_task)iterator->elem;
        
        if(t->id == rsvid)
            return rts_task_calc_budget(t);
    } 
        
    return -1;
}

float rts_scheduler_get_task_rem_budget(struct rts_scheduler* s, rsv_t rsvid) {
    struct rts_task* t;
    struct node_ptr* iterator = s->taskset->tasks->root;

    for(iterator; iterator != NULL; iterator = iterator->next) {
        t = (struct rts_task)iterator->elem;
        
        if(t->id == rsvid)
            return rts_task_calc_rem_budget(t);
    } 
        
    return -1;
}

int rts_scheduler_destroy_rsv(struct rts_scheduler* s, rsv_t rsvid) {
    struct rts_task t;
    
    t = rts_taskset_remove(&(s->taskset), rsvid); // TO BE IMPLEMENTED
    
    if(t == NULL)
        return -1;
    
    rts_scheduler_mem_detach(&(t->est_param))
    rts_task_destroy(t);
    
    for(int i = 0; i < s->num_of_plugin; i++)
        s->plg[i].calc_budget(&(s->taskset));
    
    return 0;
}

static int rts_scheduler_mem_attach(struct shatomic* mem, key_t key) {
    shatomic_init(mem);
            
    if(shatomic_use(mem, key) < 0)
        return -1;
    
    if(shatomic_attach(mem) < 0)
        return -1;
    
    return 0;
}

static void rts_scheduler_mem_detach(struct shatomic* mem) {
    shatomic_detach(mem);
}

int rts_scheduler_assign(struct rts_scheduler* s, struct rts_task t) {
    float free_budget;
    int best_plg, curr_plg;
    float best_test_result, curr_test_result;
    
      
    best_plg = -1;
    best_test_result = 0;
    free_budget = s->sys_rt_budget;
    
    // test each plugin
    for(curr_plg = 1; curr_plg < s->num_of_plugin; curr_plg++) {
        curr_test_result = s->plg[curr_plg].test(t, free_budget);
        
        if(curr_test_result > best_test_result) {
            best_test_result = curr_test_result;
            best_plg = curr_plg;
        }
        
        free_budget -= s->plg[curr_plg].used_budget;
    }
    
    // check if it is schedulable
    if(best_plg == -1)
        return -1;
    
    // choose best one
    t->sched = s->plg[best_plg].name;
    s->plg[best_plg].calc_budget(&(s->taskset));
    
    return 0;
}

int rts_scheduler_schedule(struct rts_scheduler* s, struct rts_task t) {
    for(int i = 0; i < s->num_of_plugin; i++)
        if(s->plg[i] == t->sched)
            return s->plg[i].schedule(t);
}

int rts_scheduler_deschedule(struct rts_scheduler* s, struct rts_task t) {
    for(int i = 0; i < s->num_of_plugin; i++)
        if(s->plg[i] == t->sched)
            return s->plg[i].deschedule(t);
}