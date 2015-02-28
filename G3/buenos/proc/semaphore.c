#include "proc/semaphore.h"
#include "kernel/thread.h"
#include "kernel/interrupt.h"
#include "kernel/sleepq.h"

/** @name Semaphores
 *
 * This module implements semaphores for userland.
 *
 * @{
 */

/** Table containing all semaphores in userland */
static usr_sem_t userland_sem_table[MAX_SEMAPHORES];

/** Lock which must be held before accessing the userland_sem_table */
static spinlock_t userland_sem_table_slock;

/**
 * Initializes semaphore subsystem. Sets all userland semaphores
 * as unreserved (non-existing).
 */
void userland_semaphore_init(void) {
    int i;

    spinlock_reset(&userland_sem_table_slock);
    for(i = 0; i < MAX_SEMAPHORES; i++) {
        userland_sem_table[i].creator   = -1;
        userland_sem_table[i].name[0]   = '\0';
    }
}

/**
 *
 */
usr_sem_t *find_existing_semaphore(char const* name) {
    interrupt_status_t intr_status;

    intr_status = _interrupt_disable();
    spinlock_acquire(&userland_sem_table_slock);
    for (int i = 0; i < MAX_SEMAPHORES; i++) {
        if (stringcmp(userland_sem_table[i].name, name) == 0) {
            spinlock_release(&userland_sem_table_slock);
            _interrupt_set_state(intr_status);
            return &userland_sem_table[i];
        }
    }
    spinlock_release(&userland_sem_table_slock);
    _interrupt_set_state(intr_status);
    return NULL;
}

/**
 *
 *
 * Returns NULL on error, otherwise pointer usr_sem_t which is actually a handle to the programmer
 */
usr_sem_t *create_fresh_semaphore(char const* name, int value) {
    interrupt_status_t intr_status;
    int sem_id = -1;

    intr_status = _interrupt_disable();
    spinlock_acquire(&userland_sem_table_slock);

    /* Find free semaphore from semaphore table */
    for(int i = 0; i < MAX_SEMAPHORES; i++) {
        if (userland_sem_table[i].creator == -1) {
            sem_id = i;
        }
        //A semaphore with the given name is already taken - break and return null
        if (stringcmp(userland_sem_table[i].name, name) == 0) {
            sem_id = -1;
            break;
        }
    }

    //No available semaphore - Return null
    if (sem_id < 0) {
        spinlock_release(&userland_sem_table_slock);
        _interrupt_set_state(intr_status);
        return NULL;
    }


    //Copies the name into the semaphore - Must be set here since another thread could create a semaphore with the same name
    stringcopy(userland_sem_table[sem_id].name, name, MAX_SEMAPHORE_NAME);
    userland_sem_table[sem_id].creator = thread_get_current_thread_entry()->process_id;

    spinlock_release(&userland_sem_table_slock);
    _interrupt_set_state(intr_status);


    //These actions can be done outside lock because the semaphore is not available to any other thread/process
    userland_sem_table[sem_id].value = value;
    //Makes sure the lock in the semaphore is available
    spinlock_reset(&userland_sem_table[sem_id].slock);

    return &userland_sem_table[sem_id];
}

/**
 *
 */
usr_sem_t *userland_semaphore_create(char const* name, int value) {
    if (value >= 0) {
        //Return a new semaphore
        return create_fresh_semaphore(name, value);
    } else {
        //Return a semaphore based on the name
        return find_existing_semaphore(name);
    }
}

int userland_semaphore_P(usr_sem_t *sem) {
    interrupt_status_t intr_status;

    intr_status = _interrupt_disable();
    spinlock_acquire(&sem->slock);

    //Checking that the semaphore is not destroyed
    if (sem->creator < 0) {
        spinlock_release(&sem->slock);
        _interrupt_set_state(intr_status);
        return FAILURE;
    }

    sem->value--;
    if (sem->value < 0) {
        sleepq_add(sem);
        spinlock_release(&sem->slock);
        thread_switch();
    } else {
        spinlock_release(&sem->slock);
    }
    _interrupt_set_state(intr_status);
    return 0;
}

int userland_semaphore_V(usr_sem_t *sem) {
    interrupt_status_t intr_status;

    intr_status = _interrupt_disable();
    spinlock_acquire(&sem->slock);

    //Checking that the semaphore is not destroyed
    if (sem->creator < 0) {
        spinlock_release(&sem->slock);
        _interrupt_set_state(intr_status);
        return FAILURE;
    }

    sem->value++;
    if (sem->value <= 0) {
        sleepq_wake(sem);
    }

    spinlock_release(&sem->slock);
    _interrupt_set_state(intr_status);
    return 0;
}

int userland_semaphore_destroy(usr_sem_t *sem) {
    interrupt_status_t intr_status;
    intr_status = _interrupt_disable();
    spinlock_acquire(&sem->slock);

    //Checking that the semaphore is not destroyed
    if (sem->creator < 0) {
        spinlock_release(&sem->slock);
        _interrupt_set_state(intr_status);
        return FAILURE;
    }

    if (sem->value >= 0) {
        sem->creator = -1;
        sem->name[0] = '\0';
    } else {
        spinlock_release(&sem->slock);
        _interrupt_set_state(intr_status);
        return FAILURE;
    }
    spinlock_release(&sem->slock);
    _interrupt_set_state(intr_status);
    return 0;
}