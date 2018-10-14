#include "channel.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>
#include <unistd.h>

int main() {
    struct channel c;
    char buffer[5];

    channel_create(&c, "prova", 0666);
    printf("Creato.\n");

    channel_open(&c, O_RDONLY);
    printf("Aperto.\n");
    fflush(stdout);

    while (getchar() != 'z') {
        channel_read(&c, buffer, 5);
        printf("Messaggio: %s\n", buffer);
    }

    channel_close(&c);
    channel_destroy(&c);
}