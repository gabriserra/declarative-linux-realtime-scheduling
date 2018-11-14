#include "rts_plugin.h"
#include <string.h>
#include <dlfcn.h>
#include <stdio.h>
#include <stdlib.h>


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
    }
}

static int load_libraries(struct rts_plugin* plg, int num_of_plugin) {
    void* dl_ptr;
    char so_name[NAME_MAX];
    
    for(int i = 0; i < num_of_plugin; i++) {
        strcpy(so_name, PLUGIN_PREFIX);
        strcat(so_name, PLUGIN_TO_PLUGIN_STR(plg[i].type));
        strcat(so_name, ".so");

        dl_ptr = dlopen(so_name, RTLD_NOW);
        
                char* err = dlerror();
        printf("ERROR: %s", err);

        if(dl_ptr == NULL)
            return -1;
        
        plg[i].test = dlsym(dl_ptr, TEST_FUN);

        plg[i].schedule = dlsym(dl_ptr, SCHEDULE_FUN);
        plg[i].deschedule = dlsym(dl_ptr, DESCHEDULE_FUN);
        plg[i].calc_prio = dlsym(dl_ptr, BUDGET_FUN);
        plg[i].calc_budget = dlsym(dl_ptr, PRIO_FUN);
    }
    
    return 0;
}

int rts_plugins_init(struct rts_plugin** plg, int* num) {
    FILE* f;
    int num_of_plugin;
    
    f = fopen(PLUGIN_CFG, "r");
    
    if(f == NULL)
        return -1;
    
    skip_comment(f);
    num_of_plugin = count_num_of_plugin(f);
    (*plg) = calloc(num_of_plugin, sizeof(struct rts_plugin));
    
    read_conf(f, (*plg));
    load_libraries((*plg), num_of_plugin);
    
    *num = num_of_plugin;
    fclose(f);
    
    return 0;
}

enum plugin get_plugin_from_str(char* str) {
    for (int i = 0; i < NUM_OF_SCHED; ++i)
        if (!strcmp(plugin_str[i], str))
            return INT_TO_PLUGIN(i);
    
    return NUM_OF_SCHED;
}

