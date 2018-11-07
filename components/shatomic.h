#ifndef SHATOMIC_H
#define SHATOMIC_H

#include "atomic.h"
#include <sys/ipc.h>

#define KEY_LEN 12
#define KEY_TEMPLATE ".key-XXXXXX"
#define PRIVILEGES (0666)

struct shatomic {
    int id;
    key_t key;
    int nvalue;
    atomic_t* value;
};

void shatomic_init(struct shatomic* mem);

int shatomic_create(struct shatomic* mem, int nvalue);

key_t shatomic_getkey(struct shatomic* mem);

int shatomic_use(struct shatomic* mem, key_t key);

int shatomic_attach(struct shatomic* mem);

int shatomic_destroy(struct shatomic* mem);

int shatomic_detach(struct shatomic *mem);

void shatomic_put_value(struct shatomic* mem, int index,int value);

int shatomic_get_value(struct shatomic* mem, int index);

#endif
