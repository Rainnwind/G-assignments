#include <pthread.h>

#include <assert.h>
#include <error.h>
#include <stdio.h>
#include <stdlib.h>

#include "queue.h"

int producer_count = UNINITIALIZED;
pthread_spinlock_t producer_count_lock;

void queue_init(queue_t *q) {
    node_t *new = malloc(sizeof(node_t));
    if (new == NULL) {
        perror("malloc failed");
        exit(EXIT_FAILURE);
    }

    new->next = NULL;
    pthread_mutex_init(&new->next_lock, NULL); //Initializing the next_lock with default attributes
    q->head = q->tail = new;

}

void queue_put(queue_t *q, void *item) {
    node_t *new = malloc(sizeof(node_t));
    if (new == NULL) {
        perror("malloc failed");
        exit(EXIT_FAILURE);
    }

    new->item = item;
    //Initializing mutex lock for fine grained locking system
    pthread_mutex_init(&new->next_lock, NULL); //Initializing the next_lock with default attributes
    new->next = NULL;

    //The above section has nothing to do with the list, a lock is only needed in this section
    //storing the current tail in order to release the lock again once the tail has been replaced

    node_t *old_tail = q->tail;
    //printf("Getting lock - queue_put: %p\n", (void *)&q->tail->next_lock);
    while (1) {
        pthread_mutex_lock(&old_tail->next_lock);
        if ((void *)&old_tail->next_lock != (void *)&q->tail->next_lock) {
            pthread_mutex_unlock(&old_tail->next_lock);
            old_tail = q->tail;
            continue;
//            printf("old_tail->next_lock: %p, current: %p\n", (void *)&old_tail->next_lock, (void *)&q->tail->next_lock);
  //          assert((void *)&old_tail->next_lock == (void *)&q->tail->next_lock);
        }
        break;
    }
//    printf("Locked acquired - queue_put: %p\n", (void *)&q->tail->next_lock);
    //Copying old object to keep the pointer for the mutex lock - I can do this because a lock has been acquired already and no other
    //thread can ass the current tail, once the new tail has been set, other threads may access the new tail, but the current tail
    //is locked untill released in the end

    /* add the new node to the tail */
    q->tail->next = new;
    q->tail = new;
    //Releasing the lock, making it available to get for thread_consumer to call queue_get
    pthread_mutex_unlock(&old_tail->next_lock);
  //  printf("Lock released - queue_put: %p\n-------------------\n", (void *)&old_tail->next_lock);
}

void *queue_get(queue_t *q) {
    //Lock q->head->next_lock completely - This will wait untill the resource is free
    node_t *old;
    while (1) {
        old = q->head;
        printf("Getting lock - queue_get: %p\n", (void *)&old->next_lock);
        pthread_mutex_lock(&old->next_lock);
        if (&old->next_lock == &q->head->next_lock) {
            break;
        } else {
            pthread_mutex_unlock(&old->next_lock);
            printf("Try again\n");
        }
    }

    printf("Lock acquired - queue_get: %p\n", (void *)&old->next_lock);

    /* note that the head contains a 'dummy' node. That's why we test
     * old->next. */
    if (old->next == NULL) {
        //Releasing the lock since there is nothing to protect now - Also need to unlock the lock so that other
        //threads can obtain the lock
        pthread_mutex_unlock(&old->next_lock);
        printf("Locked released - queue_get: %p\n", (void *)&old->next_lock);
        return NULL; /* queue was empty */
    }
    void *item = old->next->item;

    /* update the head and free the old memory */
    q->head = old->next;
    //The old head has be detached, therefor the old head is no longer available to any other thread and the lock can be released using
    //old->next_lock which has the same pointer to next_lock as q->head->next_lock
    pthread_mutex_unlock(&old->next_lock);
    printf("Locked released - queue_get: %p\n", (void *)&old->next_lock);
    //Destroying the mutex lock that is no longer needed and also unavailable to any other thread
    pthread_mutex_destroy(&old->next_lock);
    free(old);

    return item;
}

/**
 * thread_consumer consumes items from the queue lists and terminates when it has removed 500 elements from the queue
 *
 * @q is the pointer to the queue list that should have elements removed
 */
void *thread_consumer(void *q) {
    void *item = NULL;
    while (1) {
        item = queue_get(q);
        if (item != NULL) {
   //         printf("value get: %d\n", *(int *)item);
        } else {
            break;
        }
    }
    return NULL;
}

/**
 * thread_producer produces random items for the queue list and terminates when there are no consumers left
 *
 * @q is the pointer to the queue list that should have elements inserted
 */
void *thread_producer(void *q) {
    //Takes only a splitsecond to complete critical section which is why we use spinlock
    pthread_spin_lock(&producer_count_lock);
    if (producer_count == UNINITIALIZED) {
        producer_count = 1;
    } else {
        producer_count++;
    }
    int offset_start = (producer_count - 1) * 1000 + 1; //Gives 1, 1001, 2001, 3001, 4001 etc
    int offset_end = offset_start + 999;
    offset_start = offset_start;
    offset_end = offset_end;
    pthread_spin_unlock(&producer_count_lock);


//    for (int i = offset_start; i < offset_end; i++) {
    for (int i = 0; i < 2; i++) {
        i++;
   //     printf("value put: %d\n", i);
        queue_put(q, &i);
    }


    //Takes only a splitsecond to complete critical section which is why we use spinlock
    pthread_spin_lock(&producer_count_lock);
    producer_count--;
    pthread_spin_unlock(&producer_count_lock);
    return NULL;
}

int main(int argc, char **argv) {
    queue_t q;

    queue_init(&q);

    pthread_spin_init(&producer_count_lock, 0);

    pthread_t consumers[MAX_CONSUMERS];
    pthread_t producers[MAX_PRODUCERS];

    //queue is initialized before thread creation - Threads should not initialize the queue.
    //Initializing the spinlock


    //Spawning producers
    for (int i = 0; i < MAX_PRODUCERS; i++) {
        if (pthread_create(&producers[i], NULL, thread_producer, &q)) {
            fprintf(stderr, "Error creating thread\n");
            return 1;
        }
    }

    //Waiting for producers
    for (int i = 0; i < MAX_PRODUCERS; i++) {
        if(pthread_join(producers[i], NULL)) {
            fprintf(stderr, "Error joining thread\n");
            return 2;
        }
    }

    //Spawning consumers
    for (int i = 0; i < MAX_CONSUMERS; i++) {
        if (pthread_create(&consumers[i], NULL, thread_consumer, &q)) {
            fprintf(stderr, "Error creating thread\n");
            return 1;
        }
    }


    //Waiting for consumers
    for (int i = 0; i < MAX_CONSUMERS; i++) {
        if(pthread_join(consumers[i], NULL)) {
            fprintf(stderr, "Error joining thread\n");
            return 2;
        }
    }

    printf("done\n");
    return 0;
}