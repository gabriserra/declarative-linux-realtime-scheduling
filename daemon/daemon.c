// TODO -> sostituire tutte le printf con syslog

#include "lib/rts_daemon.h"
#include <stdio.h>
#include <signal.h>

struct rts_daemon data;

int main(int argc, char* argv[]) {
    printf("rtsdaemond - Daemon started.\n");
    
    signal(SIGINT, rts_daemon_term);
    rts_daemon_init();
    rts_daemon_loop();
}