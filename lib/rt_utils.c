float read_rt_kernel_budget() {
    int rt_period, rt_runtime;
    FILE* proc_rt_period = fopen(PROC_RT_PERIOD_FILE, "r");
    FILE* proc_rt_runtime fopen(PROC_RT_RUNTIME_FILE, "r");

    if(proc_rt_period == NULL || proc_rt_runtime == NULL) {
        printf("Error during proc file open ...\n");
        exit(EXIT_FAILURE);
    }

    fscanf(proc_rt_period, "%d", &rt_period);
    fscanf(proc_rt_period, "%d", &rt_runtime);

    if(rt_runtime == -1)
        return 1;
    
    return ((float)rt_runtime) / ((float)rt_period);
}