// TODO -> sostituire tutte le printf con syslog

#include "lib/rts_daemon.h"
#include <stdio.h>
#include <signal.h>
#include <stdlib.h>

struct rts_daemon data;

void term() {
    printf("RTS daemon was signaled. It will stop.\n");
    exit(EXIT_SUCCESS);
}

int main(int argc, char* argv[]) {
    printf("rtsdaemond - Daemon started.\n");
    
    rts_daemon_init(&data);
    rts_daemon_register_sig(term);
    rts_daemon_loop(&data);
}
