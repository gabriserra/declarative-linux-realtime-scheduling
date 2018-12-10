#include "pipe.h"
#include <unistd.h>

int pipe_open(struct pipe* p, int flags) {
    if(pipe2(p->ends, flags) < 0)
        return -1;
    return 0;
} 

void pipe_close(struct pipe* p, int end) {
    close(p->ends[end]);
}

int pipe_read(struct pipe* p, void* message, size_t size) {
    return read(p->ends[READ_FD], message, size);
}

int pipe_write(struct pipe* p, void* message, size_t size) {
    return write(p->ends[WRITE_FD], message, size);
}