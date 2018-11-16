/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

/* 
 * File:   rts_plugin.h
 * Author: gabrieleserra
 *
 * Created on November 5, 2018, 11:49 AM
 */

#ifndef RTS_PLUGIN_H
#define RTS_PLUGIN_H

#define COLUMN_MAX 82
#define NAME_MAX 25
#define PLUGIN_CFG "plugin/schedconfig.cfg"
#define PLUGIN_PREFIX "plugin/sched_"

#define TS_RECALC_UTILS_FUN "ts_recalc_utils"
#define TS_RECALC_PRIO_FUN "ts_recalc_prio"

#define T_SCHEDULE_FUN "t_schedule"
#define T_DESCHEDULE_FUN "t_deschedule"
#define T_ADD_TO_UTILS_FUN "t_add_to_utils"
#define T_REMOVE_FROM_UTILS_FUN "t_remove_from_utils"
#define T_CALC_PRIO_FUN "t_calc_prio"
#define T_TEST_FUN "t_test"

enum plugin {
    NONE,
    EDF,
    SSRM,
    DM,
    FP,
    RR,
    CUSTOM,
    NUM_OF_SCHED
};

#define INT_TO_PLUGIN(var) (enum plugin)(var)
#define INT_TO_PLUGIN_STR(var) (plugin_str[var])

#define PLUGIN_TO_INT(sched) (int)(sched)
#define PLUGIN_TO_PLUGIN_STR(sched) (plugin_str[sched])

#define PLUGIN_STR_TO_INT(str) (int) get_plugin_from_str(str)
#define PLUGIN_STR_TO_PLUGIN(str) get_plugin_from_str(str)

struct rts_task;
struct rts_taskset;

struct rts_plugin {
    int prio_min;
    int prio_max;
    int pluginid;
    
    int cpunum;
    float* util_used_percpu;
    
    enum plugin type;
    
    void (*ts_recalc_utils)(struct rts_plugin* this, struct rts_taskset* ts);
    void (*ts_recalc_prio)(struct rts_plugin* this, struct rts_taskset* ts);
    
    int (*t_schedule)(struct rts_task* t);
    int (*t_deschedule)(struct rts_task* t);
    void (*t_add_to_utils)(struct rts_plugin* this, struct rts_task* t);
    void (*t_remove_from_utils)(struct rts_plugin* this, struct rts_task* t);
    void (*t_calc_prio) (struct rts_plugin* this, struct rts_taskset* ts, struct rts_task* t);
    float (*t_test)(struct rts_plugin* this, struct rts_taskset* ts, struct rts_task* t, float* free_utils);

};

int rts_plugins_init(struct rts_plugin** plg, int* plgnum);

enum plugin get_plugin_from_str(char* str);

#endif /* RTS_PLUGIN_H */

