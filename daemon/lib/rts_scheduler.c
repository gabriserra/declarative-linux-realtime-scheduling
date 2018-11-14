#include "rts_scheduler.h"
#include "rts_taskset.h"
#include "rts_plugin.h"

static int rts_scheduler_mem_attach(struct shatomic* mem) {
    key_t key = shatomic_getkey(mem);
   
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

static int rts_scheduler_assign(struct rts_scheduler* s, struct rts_task* t) {
    float free_budget;
    int best_plg, curr_plg;
    float best_test_result, curr_test_result;
    
    best_plg = -1;
    best_test_result = 0;
    free_budget = s->sys_rt_budget;
    
    // test each plugin
    for(curr_plg = 0; curr_plg < s->num_of_plugin; curr_plg++) {
        curr_test_result = s->plugin[curr_plg].test(&(s->plugin[curr_plg]), s->taskset, t, free_budget);
        
        if(curr_test_result > best_test_result) {
            best_test_result = curr_test_result;
            best_plg = curr_plg;
        }
        
        free_budget -= s->plugin[curr_plg].used_budget;
    }
    
    // check if it is schedulable
    if(best_plg == -1)
        return -1;
    
    // choose best one
    t->plugin = s->plugin[best_plg].type;
    rts_taskset_add_top(s->taskset, t);
    
    s->plugin[best_plg].calc_prio(&(s->plugin[best_plg]), s->taskset, t);
    s->plugin[best_plg].calc_budget(&(s->plugin[best_plg]), s->taskset);
    
    return 0;
}

static int rts_scheduler_schedule(struct rts_scheduler* s, struct rts_task* t) {
    for(int i = 0; i < s->num_of_plugin; i++)
        if(s->plugin[i].type == t->plugin)
            return s->plugin[i].schedule(t);
    
    return -1;
}

static int rts_scheduler_deschedule(struct rts_scheduler* s, struct rts_task* t) {
    for(int i = 0; i < s->num_of_plugin; i++)
        if(s->plugin[i].type == t->plugin)
            return s->plugin[i].deschedule(t);
    
    return -1;
}

// PUBLIC

void rts_scheduler_init(struct rts_scheduler* s, struct rts_taskset* ts, float sys_rt_budget) {
    s->sys_rt_budget = sys_rt_budget;
    s->taskset = ts;
    s->next_rsv_id = 0;
    rts_plugins_init(&(s->plugin), &(s->num_of_plugin));
}

void rts_scheduler_delete(struct rts_scheduler* s, pid_t ppid) {   
    struct rts_task* t;
    
    while(1) {
        t = rts_taskset_remove_by_ppid(s->taskset, ppid);
        
        if(t == NULL)
            break;
        
        rts_scheduler_mem_detach(&(t->est_param));
        rts_task_destroy(t);
    }
   
    for(int i = 0; i < s->num_of_plugin; i++)
        s->plugin[i].calc_budget(&(s->plugin[i]), s->taskset);
}

float rts_scheduler_remaining_budget(struct rts_scheduler* s) {
    float free_budget = s->sys_rt_budget;
    
    for(int i = 0; i < s->num_of_plugin; i++)
        free_budget -= s->plugin[i].used_budget;
    
    return free_budget;
}

rsv_t rts_scheduler_rsv_create(struct rts_scheduler* s, struct rts_params* tp, pid_t ppid) {
    struct rts_task* t;
    
    rts_task_init(&t, 0, tp->clk);
    
    t->id = ++s->next_rsv_id;
    t->ptid = ppid;
    t->period = tp->period;
    t->wcet = tp->budget;
    t->deadline = tp->deadline;
    t->priority = tp->priority;
    t->est_param = tp->estimatedp;
    
    if(rts_scheduler_mem_attach(&(t->est_param)) < 0)
        return -1;
    if(rts_scheduler_assign(s, t) < 0)
        return -1;
        
    return s->next_rsv_id;
}

int rts_scheduler_rsv_attach(struct rts_scheduler* s, rsv_t rsvid, pid_t pid) {
    struct rts_task* t;
    iterator_t iterator;
    
    iterator = rts_taskset_iterator_init(s->taskset);
    
    for(; iterator != NULL; iterator = iterator_get_next(iterator)) {
        t = rts_taskset_iterator_get_elem(iterator);
        
        if(t->id == rsvid) {
            t->tid = pid;
            return rts_scheduler_schedule(s, t);
        }
    } 
    
    return -1;
}

int rts_scheduler_rsv_detach(struct rts_scheduler* s, rsv_t rsvid) {
    struct rts_task* t;
    iterator_t iterator;
    
    iterator = rts_taskset_iterator_init(s->taskset);
    
    for(; iterator != NULL; iterator = iterator_get_next(iterator)) {
        t = rts_taskset_iterator_get_elem(iterator);
        
        if(t->id == rsvid)
            return rts_scheduler_deschedule(s, t);
    } 
    
    return -1;
}

float rts_scheduler_rsv_budget(struct rts_scheduler* s, rsv_t rsvid) {
    struct rts_task* t;
    iterator_t iterator;

    iterator = rts_taskset_iterator_init(s->taskset);
    
    for(; iterator != NULL; iterator = iterator_get_next(iterator)) {
        t = rts_taskset_iterator_get_elem(iterator);
        
        if(t->id == rsvid)
            return rts_task_calc_budget(t);
    } 
        
    return -1;
}

float rts_scheduler_rsv_rem_budget(struct rts_scheduler* s, rsv_t rsvid) {
    struct rts_task* t;
    iterator_t iterator;

    iterator = rts_taskset_iterator_init(s->taskset);
    
    for(; iterator != NULL; iterator = iterator_get_next(iterator)) {
        t = rts_taskset_iterator_get_elem(iterator);
        
        if(t->id == rsvid)
            return rts_task_calc_rem_budget(t);
    } 
        
    return -1;
}

int rts_scheduler_rsv_destroy(struct rts_scheduler* s, rsv_t rsvid) {
    struct rts_task* t;
    
    t = rts_taskset_remove_by_rsvid(s->taskset, rsvid);
    
    if(t == NULL)
        return -1;
    
    rts_scheduler_mem_detach(&(t->est_param));
    rts_task_destroy(t);
    
    for(int i = 0; i < s->num_of_plugin; i++)
        s->plugin[i].calc_budget(&(s->plugin[i]), s->taskset);
    
    return 0;
}