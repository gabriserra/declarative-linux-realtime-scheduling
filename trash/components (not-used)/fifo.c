#include "fifo.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>
#include <unistd.h>

static void* alloc(int count, size_t dimension) {
    void* ret = calloc(count, dimension);

    if(ret != NULL)
        return ret;

    printf("The system is out of memory!");
    exit(-1);
}

void fifo_create(struct fifo* c, char* name, mode_t auth) {
    int ret = mkfifo(name, auth);

    if(ret < 0) {
        printf("Unable to create communication fifo. Terminate.");
        exit(-1);
    }

    c->name = alloc(strlen(name)+1, 1);
    strcpy(c->name, name);
}

void fifo_use(struct fifo* c, char* name) {
    c->auth = 0;
    c->descriptor = 0;
    c->name = alloc(strlen(name)+1, 1);
    strcpy(c->name, name);
}

void fifo_open(struct fifo* c, int mode) {
    int fd = open(c->name, mode);

    if(fd < 0) {
        printf("Unable to create communication fifo. Terminate."); //TODO - sys log or something similar
        exit(-1);
    }

    c->descriptor = fd;
}

void fifo_close(struct fifo* c) {
    close(c->descriptor);
}

void fifo_destroy(struct fifo* c) {
    unlink(c->name);
    free(c->name);
    memset(c, 0, sizeof(struct fifo));
}

int fifo_read(struct fifo* c, void* message, size_t size) {
    ssize_t n = read(c->descriptor, message, size);

    if(n != size)
        return -1; 
    
    return 0;
}

int fifo_write(struct fifo* c, void* message, size_t size) {
    ssize_t n = write(c->descriptor, message, size);

    if(n != size)
        return -1; 
    
    return 0;
}

