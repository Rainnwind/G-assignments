#ifndef BUENOS_USERLAND_SEMAPHORE_H
#define BUENOS_USERLAND_SEMAPHORE_H

#define MAX_SEMAPHORES 128
#define MAX_SEMAPHORE_NAME 256

#define FAILURE -1

#include "kernel/spinlock.h"
#include "proc/process.h"

typedef struct {
    char name[MAX_SEMAPHORE_NAME];
    spinlock_t slock;
    int value;
    process_id_t creator;
} usr_sem_t;

void userland_semaphore_init(void);
usr_sem_t *userland_semaphore_create(char const* name, int value);
int userland_semaphore_P(usr_sem_t *sem);
int userland_semaphore_V(usr_sem_t *sem);
int userland_semaphore_destroy(usr_sem_t *sem);

#endif /* BUENOS_USERLAND_SEMAPHORE_H */
