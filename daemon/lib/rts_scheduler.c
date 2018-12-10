#include "rts_scheduler.h"
#include "rts_taskset.h"
#include "rts_task.h"
#include "rts_plugin.h"
#include <sys/sysinfo.h>
#include <stdlib.h>
#include <string.h>

static int rts_scheduler_mem_attach(struct shatomic* mem) {
    key_t key = shatomic_getkey(mem);
   
    shatomic_init(mem);
            
    if(shatomic_use(mem, key, mem->nvalue) < 0)
        return -1;
    
    if(shatomic_attach(mem) < 0)
        return -1;
    
    return 0;
}

static void rts_scheduler_mem_detach(struct shatomic* mem) {
    shatomic_detach(mem);
}

static void rts_scheduler_add_utils(struct rts_scheduler* s, struct rts_task* t) {
    s->sys_rt_curr_free_utils[t->cpu] -= rts_task_get_util(t);
}

static void rts_scheduler_remove_utils(struct rts_scheduler* s, struct rts_task* t) {
    s->sys_rt_curr_free_utils[t->cpu] += rts_task_get_util(t);
}

static int rts_scheduler_assign(struct rts_scheduler* s, struct rts_task* t) {
    int best_cpu;
    int best_plg;
    int curr_plg;
    float best_test;
    float curr_test;
    
    best_plg = -1;
    best_test = 0;
    
    for(curr_plg = 0; curr_plg < s->num_of_plugin; curr_plg++) {
        
        curr_test = s->plugin[curr_plg].t_test(&(s->plugin[curr_plg]), 
                    s->taskset, t, s->sys_rt_curr_free_utils);
        
        if(curr_test > best_test) {
            best_test = curr_test;
            best_plg = curr_plg;
            best_cpu = t->cpu;
        }
        
    }
    
    if(best_plg == -1)
        return -1;
    
    t->cpu = best_cpu;
    t->pluginid = best_plg;
    rts_taskset_add_top(s->taskset, t);
    
    s->plugin[best_plg].t_calc_prio(&(s->plugin[best_plg]), s->taskset, t);
    s->plugin[best_plg].t_add_to_utils(&(s->plugin[best_plg]), t);
    
    rts_scheduler_add_utils(s, t);
    
    return 0;
}

static int rts_scheduler_schedule(struct rts_scheduler* s, struct rts_task* t) {
    return s->plugin[t->pluginid].t_schedule(t);
}

static int rts_scheduler_deschedule(struct rts_scheduler* s, struct rts_task* t) {
    return s->plugin[t->pluginid].t_deschedule(t);
}

// PUBLIC

void rts_scheduler_init(struct rts_scheduler* s, struct rts_taskset* ts, int rt_period, int rt_runtime) {
    int i;
    float sys_rt_util;
    
    s->sys_rt_period = rt_period;
    s->sys_rt_runtime = rt_runtime;
    
    if(rt_runtime == -1)
        sys_rt_util = 1;
    else
        sys_rt_util = s->sys_rt_runtime / (float)s->sys_rt_period;
    
    s->num_of_cpu = get_nprocs();
    s->sys_rt_free_utils = calloc(s->num_of_cpu, sizeof(float));
    s->sys_rt_curr_free_utils = calloc(s->num_of_cpu, sizeof(float));
    
    for(i = 0; i < s->num_of_cpu; i++) {
        s->sys_rt_free_utils[i] = sys_rt_util;
        s->sys_rt_curr_free_utils[i] = sys_rt_util;
    }
    
    s->taskset = ts;
    s->next_rsv_id = 0;
    rts_plugins_init(&(s->plugin), &(s->num_of_plugin));
}

void rts_scheduler_destroy(struct rts_scheduler* s) {
    free(s->sys_rt_free_utils);
    free(s->sys_rt_curr_free_utils);
    rts_plugins_destroy(s->plugin, s->num_of_plugin);
}

void rts_scheduler_delete(struct rts_scheduler* s, pid_t ppid) {   
    struct rts_task* t;
    
    while(1) {
        t = rts_taskset_remove_by_ppid(s->taskset, ppid);
        
        if(t == NULL)
            break;
        
        rts_scheduler_remove_utils(s, t);
        s->plugin[t->pluginid].t_remove_from_utils(&(s->plugin[t->pluginid]), t);
        
        rts_scheduler_mem_detach(&(t->est_param));
        rts_task_destroy(t);
    }
}

int rts_scheduler_refresh_utils(struct rts_scheduler* s) {
    int cpus_overl = 0;
    
    for(int i = 0; i < s->num_of_plugin; i++)
        cpus_overl -= s->plugin[i].ts_recalc_utils(&(s->plugin[i]), s->taskset);
    
    return cpus_overl;
}

void rts_scheduler_refresh_prios(struct rts_scheduler* s) {
    for(int i = 0; i < s->num_of_plugin; i++)
        s->plugin[i].ts_recalc_prios(&(s->plugin[i]), s->taskset);
}

int rts_scheduler_refresh_util(struct rts_scheduler* s, struct rts_task* t) {
    return s->plugin[t->pluginid].t_recalc_util(&(s->plugin[t->pluginid]), t);
}

void rts_scheduler_refresh_prio(struct rts_scheduler* s, struct rts_task* t) {    
    s->plugin[t->pluginid].t_calc_prio(&(s->plugin[t->pluginid]), s->taskset, t);
}

float rts_scheduler_get_free_util(struct rts_scheduler* s) {
    float sys_util = 0;
    
    for(int i = 0; i < s->num_of_cpu; i++)
        sys_util += s->sys_rt_free_utils[i];
    
    return sys_util / s->num_of_cpu;
}

float rts_scheduler_get_remaining_util(struct rts_scheduler* s) {
    float free_util = 0;
    
    for(int i = 0; i < s->num_of_cpu; i++)
        free_util += s->sys_rt_curr_free_utils[i];
    
    return free_util / s->num_of_cpu;
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
    
    rts_task_update_util(t);
    t->util = rts_task_get_util(t);
    
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

int rts_scheduler_rsv_destroy(struct rts_scheduler* s, rsv_t rsvid) {
    struct rts_task* t;
    
    t = rts_taskset_remove_by_rsvid(s->taskset, rsvid);
    
    if(t == NULL)
        return -1;
    
    rts_scheduler_remove_utils(s, t);
    s->plugin[t->pluginid].t_remove_from_utils(&(s->plugin[t->pluginid]), t);
    
    rts_scheduler_mem_detach(&(t->est_param));
    rts_task_destroy(t);
        
    return 0;
}