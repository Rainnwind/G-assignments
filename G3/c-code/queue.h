#ifndef QUEUE_GUARD
#define QUEUE_GUARD

#define DONE 0
#define UNINITIALIZED -1

#define MAX_CONSUMERS 100
#define MAX_PRODUCERS 10

typedef struct node {
    pthread_mutex_t next_lock;  //Used for fined grain locking
	void *item;
	struct node *next;
} node_t;

typedef struct queue {
	node_t	*head;
	node_t	*tail;
} queue_t;

/**
 * queue_init initializes a new queue
 *
 * It is expected that this function is only called once.
 */
void queue_init(queue_t *q);

/**
 * queue_put adds the item to the tail of the queue
 */
void queue_put(queue_t *q, void *item);

/**
 * queue_get removes and returns the head of the queue in item
 */
void *queue_get(queue_t *q);

#endif //QUEUE_GUARD