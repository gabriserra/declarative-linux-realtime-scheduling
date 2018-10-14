

/*

start del demone
check plugin disponibili?
creo file mknode fifo

loop attendo (sul file? o aspetto signal?)

creo task con caratteristiche richieste
aggiungo task al taskset corrente

leggo max fattore utilizzazione su proc

- SE CHIESTO ESPLICITAMENTE USARE UN PLUGIN?
- SE NESSUN TASK SPECIFICA PRIO/DEADLINE/PERIODO -> SCHED RR
- SE ALMENO UNO SPECIFICA PRIORITA -> SCHED FIFO
- SE ALMENO UNO SPECIFICA PRIO/PERIODO -> RM o EDF (OVER SCHED_FIFO)
- SE ALMENO UNO SPECIFICA PRIO/DEADLINE = PERIODO -> RM o EDF (OVER SCHED_FIFO)
- SE ALMENO UNO SPECIFICA PRIO/DEADLINE != PERIODO -> DM o EDF (OVER SCHED_FIFO)
- SE ALMENO UNO SPECIFICA ISOLAMENTO TEMPORALE -> SPORADIC (OVER SCHED_DEADLINE)

- per ogni item, se supero fattore U max, rigetto la richiesta
- per ogni item, se non supero fattore U max ma il test di schedulabilità fallisce
  provo a passare ad EDF (se non voglio isolamento temporale)

*/

#include "assets/channel.h"
#include <stdlib.h>


int main(void) {

    printf("Start del demone.\n");
    // check plugin disponibili
    fifo_create();

    while(1) {
        
        
        sleep(1);
    }
}

void fifo_create() {
    int ret;

    ret = mkfifo("file", 0666);

    if(ret < 0) {
        printf("Unable to create communication channel. Terminate.");
        return;
    }
}

void fifo_read_b(int fd) {
    // Open FIFO for Read only 
    fd = open(myfifo, O_RDONLY); 
  
    // Read from FIFO 
    read(fd, arr1, sizeof(arr1)); 
  
    // Print the read message 
    printf("User2: %s\n", arr1); 
    close(fd); 
}