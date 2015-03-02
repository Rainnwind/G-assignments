#ifndef QUEUE_GUARD
#define QUEUE_GUARD

#include <pthread.h>

#define MAX_INIT_SIZE 1

#define PUTTERS_UNINITIALIZED -1
#define DONE 0

#define MAX_PUTTERS 40
#define MAX_GETTERS 40

typedef struct {
    void *item;
    pthread_mutex_t lock;
} node_t;

typedef struct {
    node_t *array;
    int max_size;
    pthread_mutex_t update_array_lock;
    pthread_cond_t update_condition;
    pthread_mutex_t users_lock;
    pthread_cond_t users_condition;
    int users;
    int is_updating;
} list_t;

/**
 * Initializes an array, must be used before using the list in a multithread
 * enviroment.
 *
 * @list is the list_t that's going to be initializes
 */
void queue_init_list_t(list_t *list);

/**
 * Puts an item into the lock, tries to lock every entry in the table until it finds one
 * it can lock, un success it will check if the item is NULL, only if it is NULL will it insert
 * the item, otherwise it will keep searching
 *
 * If it fails to find a slut it will try to expand the array and then reinsert the item
 *
 * @list is the list that is intended to put item in
 * @item is the item to put in the list, it can be anything.
 *      It is expected to be allocated space before this function is called
 */
void queue_put_list_t(list_t *list, void *item);

/**
 * Collects the first item that's is not NULL and obtained by a lock
 * Returns pointer to item on success other NULL
 *
 * @list is the list that is taken an item from
 */
void *queue_get_list_t(list_t *list);

/**
 * This is called by queue_put_list_t when it fails to insert an item into the list
 * It expands the array in the list_t object by a factor of 2 i.e. size 2 is becomes 4, 4 becomes 8 and so on
 * When this function has ended it will call queue_put_list_t in order to put in the item it failed to stock in the array
 *
 * @list is the list that must be expanded
 * @item is the object that was not put into the list
 */
void queue_expand_list_t(list_t *list, void *item);

#endif //QUEUE_GUARD
