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

#define TEST_FUN "test"
#define SCHEDULE_FUN "schedule"
#define DESCHEDULE_FUN "deschedule"
#define BUDGET_FUN "calc_budget"
#define PRIO_FUN "calc_prio"

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
    enum plugin type;
    float used_budget;
    
    float (*test)(struct rts_plugin* this, struct rts_taskset* ts, struct rts_task* t, float free_budget);
    int (*schedule)(struct rts_task* t);
    int (*deschedule)(struct rts_task* t);
    void (*calc_prio) (struct rts_plugin* this, struct rts_taskset* ts, struct rts_task* t);
    void (*calc_budget)(struct rts_plugin* this, struct rts_taskset* ts);
};

int rts_plugins_init(struct rts_plugin** plg, int* num);

enum plugin get_plugin_from_str(char* str);

#endif /* RTS_PLUGIN_H */

