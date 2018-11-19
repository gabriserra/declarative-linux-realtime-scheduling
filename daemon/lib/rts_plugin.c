#include "rts_plugin.h"
#include <string.h>
#include <dlfcn.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/sysinfo.h>

static const char *plugin_str[] = {
    "NONE",
    "EDF",
    "SSRM",
    "DM",
    "FP",
    "RR",
    "CUSTOM"
};

static void skip_comment(FILE* f) {
    char buffer[COLUMN_MAX];
    
    while(1) {
        fgets(buffer, COLUMN_MAX, f);
        
        if(buffer[0] == '!')
            break;
    } 
}

static int count_num_of_plugin(FILE* f) {
    long start_pos;
    int num_of_alg;
    char buffer[COLUMN_MAX];
    
    start_pos = ftell(f);
    num_of_alg = 0;
    
    while(!feof(f)) {
        fgets(buffer, COLUMN_MAX, f);
        num_of_alg++;
    }
        
    fseek(f, start_pos, SEEK_SET);
    return num_of_alg;
}

static void read_conf(FILE* f, struct rts_plugin* plg) {
    char buffer[COLUMN_MAX];
    int imp;
    
    while(!feof(f)) {
        fscanf(f, "%d", &imp);
        fscanf(f, "%s", buffer);
        fscanf(f, "%d%*[-/]%d", &(plg[imp].prio_min), &(plg[imp].prio_max));
        plg[imp].type = get_plugin_from_str(buffer);
        plg[imp].pluginid = imp;
    }
}

static int load_libraries(struct rts_plugin* plg, int num_of_plugin) {
    void* dl_ptr;
    char so_name[NAME_MAX];
    int cpunum = get_nprocs();
    
    for(int i = 0; i < num_of_plugin; i++) {
        strcpy(so_name, PLUGIN_PREFIX);
        strcat(so_name, PLUGIN_TO_PLUGIN_STR(plg[i].type));
        strcat(so_name, ".so");

        dl_ptr = dlopen(so_name, RTLD_NOW);
        
        if(dl_ptr == NULL)
            return -1;
        
        plg[i].cpunum = cpunum;
        plg[i].dl_ptr = dl_ptr;
        plg[i].util_used_percpu = calloc(cpunum, sizeof(float));        
        plg[i].pluginid = i;
        
        plg[i].ts_recalc_utils = dlsym(dl_ptr, TS_RECALC_UTILS_FUN);
        plg[i].ts_recalc_prios = dlsym(dl_ptr, ts_recalc_prios_FUN);
        plg[i].t_schedule = dlsym(dl_ptr, T_SCHEDULE_FUN);
        plg[i].t_deschedule = dlsym(dl_ptr, T_DESCHEDULE_FUN);
        plg[i].t_add_to_utils = dlsym(dl_ptr, T_ADD_TO_UTILS_FUN);
        plg[i].t_recalc_util = dlsym(dl_ptr, T_RECALC_UTIL_FUN);
        plg[i].t_remove_from_utils = dlsym(dl_ptr, T_REMOVE_FROM_UTILS_FUN);
        plg[i].t_calc_prio = dlsym(dl_ptr, T_CALC_PRIO_FUN);
        plg[i].t_test = dlsym(dl_ptr, T_TEST_FUN);
    }
    
    return 0;
}

int rts_plugins_init(struct rts_plugin** plgs, int* plgnum) {
    FILE* f;
    int num_of_plugin;
    
    f = fopen(PLUGIN_CFG, "r");
    
    if(f == NULL)
        return -1;
    
    skip_comment(f);
    num_of_plugin = count_num_of_plugin(f);
    (*plgs) = calloc(num_of_plugin, sizeof(struct rts_plugin));
        
    read_conf(f, (*plgs));
    load_libraries((*plgs), num_of_plugin);
    
    *plgnum = num_of_plugin;
    fclose(f);
    
    return 0;
}

void rts_plugins_destroy(struct rts_plugin* plgs, int plgnum) {    
    for(int i = 0; i < plgnum; i++) {
        free(plgs[i].util_used_percpu);
        dlclose(plgs[i].dl_ptr);
    }
    
    free(plgs);
}

enum plugin get_plugin_from_str(char* str) {
    for (int i = 0; i < NUM_OF_SCHED; ++i)
        if (!strcmp(plugin_str[i], str))
            return INT_TO_PLUGIN(i);
    
    return NUM_OF_SCHED;
}

