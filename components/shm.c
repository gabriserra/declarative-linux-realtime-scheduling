#include "shm.h"
#include <sys/ipc.h>
#include <sys/shm.h>
#include <atomic.h>


struct shm {
    atomic_t count;
};

#define PRIVILEGES (0666)

static inline int getkey(char* keyfile, char proj_id) {
    return ftok(keyfile, proj_id);
}

struct shared_memory* _shmem_get(char* keyfile, char proj_id, int flags)
{
    int key = getkey(keyfile, proj_id);

    int shmid = shmget(key, sizeof(struct shared_memory), PRIVILEGES | flags);

    if (shmid < 0)
        return NULL;

    struct shared_memory* self = shmat(shmid, NULL, 0);

    self->shmem_id = shmid;

    return self;
}

static int _shmem_destroy(struct shared_memory* self)
{
    return shmctl(self->shmem_id, IPC_RMID, NULL);
}

struct shared_memory *shmem_create(char *keyfile, char proj_id)
{
    struct shared_memory* self = _shmem_get(keyfile, proj_id, IPC_CREAT | IPC_EXCL);

    if (self == NULL)
        return NULL;

    // Initializing the memory area
    self->how_many_data = 0;
    self->how_many_free = SHM_BUFFER_SIZE;
    self->front = 0;
    self->rear = 0;
    self->expect_more_data = true;

    // Initialize mutex and condition variables
    int res = pthread_once(&attributes_are_initialized, _attr_init);

    if (res != 0)
    {
        _shmem_destroy(self);

        errno = res;    // I set it here to override the case in which the shmctl
                        // overrides errno

        return NULL;
    }

    pthread_mutex_init(&self->mutex, &mutex_attrs);
    pthread_cond_init(&self->nonempty, &cond_attrs);
    pthread_cond_init(&self->nonfull, &cond_attrs);

    return self;
}


int shmem_destroy(struct shared_memory *self)
{
    // Should destroy mutexes and condition variables, but if they are used I
    // cannot... I just hope the destruction of the shared memory by the last
    // user will not make any damages (further investigation required)
    return _shmem_destroy(self);
}

struct shared_memory *shmem_get(char *keyfile, char proj_id)
{
    return _shmem_get(keyfile, proj_id, 0);
}

int shmem_unget(struct shared_memory *self)
{
    return shmdt(self);
}

static inline int index_plusplus(int* index)
{
    int value = *index;
    *index = (*index + 1) % SHM_BUFFER_SIZE;
    return value;
}

// NOTICE: The following functions work even with multiple producers/consumers,
// but only one producer can invoke the shmem_end_comm
void shmem_put_data(struct shared_memory* self, char value)
{
    int index = 0;

    pthread_mutex_lock(&self->mutex);

    // Until the buffer is full, I need to wait
    while(self->how_many_free == 0)
    {
        pthread_cond_wait(&self->nonfull, &self->mutex);
    }

    // There is some space
    index = index_plusplus(&self->front);
    --self->how_many_free;

    pthread_mutex_unlock(&self->mutex);

    // I can write in the position given by index
    self->data[index] = value;

    // Since there is new data I need to increase count and signal the consumer
    pthread_mutex_lock(&self->mutex);
    ++self->how_many_data;
    pthread_cond_signal(&self->nonempty);
    pthread_mutex_unlock(&self->mutex);
}

bool shmem_get_data(struct shared_memory* self, char* value)
{
    int index = 0;

    pthread_mutex_lock(&self->mutex);

    // Until the buffer is empty, I need to wait
    while (self->how_many_data == 0 && self->expect_more_data)
    {
        pthread_cond_wait(&self->nonempty, &self->mutex);
    }

    if (self->expect_more_data == false && self->how_many_data == 0)
    {
        // I need to exit with an error
        pthread_mutex_unlock(&self->mutex);
        return false;
    }

    // There is some data
    index = index_plusplus(&self->rear);
    --self->how_many_data;

    pthread_mutex_unlock(&self->mutex);

    // I can read from the position given by index
    *value = self->data[index];

    // Since there is a new emtpy space now I need to decrease count and signal
    // the producer
    pthread_mutex_lock(&self->mutex);
    ++self->how_many_free;
    pthread_cond_signal(&self->nonfull);
    pthread_mutex_unlock(&self->mutex);

    return true;
}

void shmem_end_comm(struct shared_memory* self)
{
    // Since there may be a consumer waiting for non-existing data, I need to
    // signal him
    pthread_mutex_lock(&self->mutex);
    self->expect_more_data = false;
    pthread_cond_signal(&self->nonempty);
    pthread_mutex_unlock(&self->mutex);
}
