#ifndef SHM_H
#define SHM_H

struct shm;

struct shm* shm_create(char* keyfile, char proj_id);

int shm_destroy(struct shared_memory* shmem_ptr);

struct shm *shm_get(char *keyfile, char proj_id);

int shm_unget(struct shared_memory *shmem_ptr);

void shm_put_data(struct shared_memory* shmem_ptr, char value);

int shm_get_data(struct shared_memory* shmem_ptr, char* value);

void shm_end_comm(struct shared_memory* shmem_ptr);

#endif
