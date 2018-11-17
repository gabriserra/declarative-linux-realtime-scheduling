// TODO -> sostituire tutte le printf con syslog

#include "lib/rts_daemon.h"
#include <stdio.h>
#include <signal.h>
#include <stdlib.h>

struct rts_daemon data;

void term() {
    printf("\nRTS daemon was signaled. It will destroy data and stop.\n");
    rts_daemon_destroy(&data);
    exit(EXIT_SUCCESS);
}

void exit_err(char* str) {
    printf("%s", str);
    exit(EXIT_FAILURE);
}

int main(int argc, char* argv[]) {
    printf("rtsdaemond - Daemon started.\n");
    
    if(rts_daemon_init(&data) < 0)
        exit_err("Something gone wrong in the init phase.\n");
    
    rts_daemon_register_sig(term);
    rts_daemon_loop(&data);
    
    return 0;
}
