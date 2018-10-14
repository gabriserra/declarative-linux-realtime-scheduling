#include "channel.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>
#include <unistd.h>

int main() {
    struct channel c;
    char buffer[5];

    channel_use(&c, "prova");
    channel_open(&c, O_WRONLY);
    printf("Aperto.\n");

    strcpy(buffer, "ciao\0");

    while (getchar() != 'z') {
        channel_write(&c, buffer, 5);
        printf("Inviato!\n");
    }

    channel_close(&c);
}