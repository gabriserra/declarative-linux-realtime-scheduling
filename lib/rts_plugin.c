#include <string.h>
#include <dlfcn.h>

#define PLUGIN_PATH "plugin"
#define COLUMN_MAX 82
#define NAME_MAX 15

#define TEST_FUN "test"
#define SCHEDULE_FUN "schedule"
#define DESCHEDULE_FUN "deschedule"
#define BUDGET_FUN "calc_budget"

struct rts_sched_plugin {
    enum SCHED name;
    int prio_min;
    int prio_max;
    float used_budget;
    float (*test)(struct rts_task* t, float free_budget);
    void (*schedule)(struct rts_task* t);
    void (*deschedule)(struct rts_task* t);
    void (*calc_budget)(struct rts_taskset* t);
};

int rts_plugins_init(struct rts_sched_plugin* plg, int* num) {
    FILE* f;
    int num_of_plugin;
    
    f = fopen(PLUGIN_PATH, "r");
    
    if(f == NULL)
        return -1;
    
    skip_comment(f);
    num_of_plugin = count_num_of_plugin(f);
    plg = calloc(num_of_plugin, sizeof(struct rts_sched_plugin));
    
    read_conf(f, plg);
    load_libraries(plg, num_of_plugin);
    
    *num = num_of_plugin;
    fclose(f);
}

int load_libraries(struct rts_sched_plugin* plg, int num_of_plugin) {
    void* dl_ptr;
    char so_name[NAME_MAX];
    
    for(int i = 0; i < num_of_plugin; i++) {
        strcpy(so_name, stringfy(plg[i].type));
        strcat(so_name, ".so");

        dl_ptr = dlopen(so_name, RTLD_NOW);

        if(dl_ptr == NULL)
            return -1;
        
        plg[i].test = dlsym(dl_ptr, TEST_FUN);
        plg[i].schedule = dlsym(dl_ptr, SCHEDULE_FUN);
        plg[i].deschedule = dlsym(dl_ptr, DESCHEDULE_FUN);
        plg[i].test = dlsym(dl_ptr, BUDGET_FUN);
    }
    
    return 0;
}

void skip_comment(FILE* f) {
    char buffer[COLUMN_MAX];
    
    while(1) {
        fgets(buffer, COLUMN_MAX, f);
        
        if(buffer[0] != '#')
            break;
    } 
}

int count_num_of_plugin(FILE* f) {
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

enum SCHED schedname_to_enum(char* string) {
    int i;
    
    for(i = 0; i < NUM_OF_SCHED; i++)
        if(strcmp(stringfy(SCHED(i)), string))
            return SCHED(i);
    
    return NUM_OF_SCHED;
}

void read_conf(FILE* f, struct rts_sched_plugin* plg) {
    char buffer[COLUMN_MAX];
    int imp;
    
    while(!feof(f)) {
        fscanf(f, "%d", &imp);
        fscanf(f, "%s", buffer);
        fscanf(f, "%d%*[-/]%d", &(plg[imp].prio_min), &(plg[imp].prio_max));
        plg[imp].name = schedname_to_enum(buffer);
    }
}

