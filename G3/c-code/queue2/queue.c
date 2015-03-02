#include <assert.h>
#include <stdlib.h>
#include <stdio.h>
#include "queue.h"

int putters_count = PUTTERS_UNINITIALIZED;
pthread_mutex_t putters_lock = PTHREAD_MUTEX_INITIALIZER;

//used for testing
int put_count = 0;
pthread_mutex_t put_lock = PTHREAD_MUTEX_INITIALIZER;

//used for testing
int get_count = 0;
pthread_mutex_t get_lock = PTHREAD_MUTEX_INITIALIZER;

void queue_init_list_t(list_t *list) {
    node_t *tmp_array = (node_t *)malloc(MAX_INIT_SIZE * (sizeof(node_t)));
    if (tmp_array == NULL) {
        exit(1);
    }
    list->array = tmp_array;
    for (int i = 0; i < MAX_INIT_SIZE; i++) {
        list->array[i].item = NULL;
        pthread_mutex_init(&list->array[i].lock, NULL);
    }
    pthread_mutex_init(&list->update_array_lock, NULL);
    pthread_cond_init(&list->update_condition, NULL);
    pthread_mutex_init(&list->users_lock, NULL);
    pthread_cond_init(&list->users_condition, NULL);
    list->users = 0;
    list->is_updating = 0;
    list->max_size = MAX_INIT_SIZE;
}


void queue_put_list_t(list_t *list, void *item) {
    pthread_mutex_lock(&list->update_array_lock);
    while (list->is_updating != 0) {
        pthread_cond_wait(&list->update_condition, &list->update_array_lock);
    }
    pthread_mutex_unlock(&list->update_array_lock);

    pthread_mutex_lock(&list->users_lock);
    list->users++;
    pthread_mutex_unlock(&list->users_lock);

    int found = 0;
    int max_size = list->max_size;
    for (int i = 0; i < max_size; i++) {
        //Trylocking to prevent blocking the thread since it's looking for an available slot in the array
        if (pthread_mutex_trylock(&list->array[i].lock) == 0) {
            //It's not allowed to override elements in the list
            if (list->array[i].item == NULL) {
                list->array[i].item = item;
                found = 1;
                //We are done, release lock and return asap
                pthread_mutex_unlock(&list->array[i].lock);
                break;
            }
            //Keep trying
            pthread_mutex_unlock(&list->array[i].lock);
        }
    }
    pthread_mutex_lock(&list->users_lock);
    list->users--;
    pthread_cond_signal(&list->users_condition);
    pthread_mutex_unlock(&list->users_lock);

    if (found == 0) {
        queue_expand_list_t(list, item);
    }
}



void *queue_get_list_t(list_t *list) {
    void *ret_item = NULL;
    pthread_mutex_lock(&list->update_array_lock);
    while (list->is_updating != 0) {
        pthread_cond_wait(&list->update_condition, &list->update_array_lock);
    }
    pthread_mutex_unlock(&list->update_array_lock);

    pthread_mutex_lock(&list->users_lock);
    list->users++;
    pthread_mutex_unlock(&list->users_lock);


    int max_size = list->max_size;
    for (int i = 0; i < max_size; i++) {
        if (pthread_mutex_trylock(&list->array[i].lock) == 0) {
            if (list->array[i].item != NULL) {
                ret_item = list->array[i].item;
                list->array[i].item = NULL;
                pthread_mutex_unlock(&list->array[i].lock);
                break;
            }
            //Keep trying
            pthread_mutex_unlock(&list->array[i].lock);
        }
    }
    pthread_mutex_lock(&list->users_lock);
    list->users--;
    pthread_cond_signal(&list->users_condition);
    pthread_mutex_unlock(&list->users_lock);

    //List was empty
    return ret_item;
}

void queue_expand_list_t(list_t *list, void *item) {
    int i_update = 0;
    pthread_mutex_lock(&list->update_array_lock);
    if (list->is_updating == 0) {
        list->is_updating = 1;
        i_update = 1;
    }
    pthread_mutex_unlock(&list->update_array_lock);
    if (i_update == 1) {
        pthread_mutex_lock(&list->users_lock);
        while (list->users > 0) {
            pthread_cond_wait(&list->users_condition, &list->users_lock);
        }
        pthread_mutex_unlock(&list->users_lock);

        if (i_update == 1) {
            printf("Expanding the list\n");
            //Do the updating stuff...
            int new_maxsize = list->max_size * 2;

            node_t *tmp_array = realloc(list->array, new_maxsize * (sizeof(node_t)));
            if (tmp_array == NULL) {
                exit(2);
            }
            list->array = tmp_array;
            for (int i = list->max_size; i < new_maxsize; i++) {
                list->array[i].item = NULL;
                pthread_mutex_init(&list->array[i].lock, NULL);
            }
            list->max_size = new_maxsize;

            list->is_updating = 0;
            pthread_cond_broadcast(&list->update_condition);
        }
    }

    queue_put_list_t(list, item);
}

void *thread_putter(void *list) {
    pthread_mutex_lock(&putters_lock);
    if (putters_count == PUTTERS_UNINITIALIZED) {
        putters_count = 1;
    } else {
        putters_count++;
    }
    pthread_mutex_unlock(&putters_lock);

    //used for testing
    pthread_mutex_lock(&put_lock);
    put_count++;
    pthread_mutex_unlock(&put_lock);


    for (int i = 0; i < 500; i++) {
        int *item = (int *)malloc(sizeof(int));
        *item = 2;
        queue_put_list_t(list, item);
    }

    pthread_mutex_lock(&putters_lock);
    putters_count--;
    pthread_mutex_unlock(&putters_lock);
    return NULL;
}

void *thread_getter(void *list) {
    while (putters_count == PUTTERS_UNINITIALIZED) {
        //Do nothing - Should only be for a short while
    }
    while (putters_count != DONE) {

        void *item2 = queue_get_list_t(list);
        if (item2 == NULL) {
            printf("ffs....\n");
        } else {
            printf("Uhh... I've got it :): %d\n", *(int *)item2);

            //used for testing
            pthread_mutex_lock(&get_lock);
            get_count++;
            pthread_mutex_unlock(&get_lock);

        }
    }
    return NULL;
}



int main() {
    list_t list;
    queue_init_list_t(&list);


    pthread_t putters[MAX_PUTTERS];
    pthread_t getters[MAX_GETTERS];

    //Creating getters
    for (int i = 0; i < MAX_GETTERS; i++) {
        if (pthread_create(&getters[i], NULL, thread_getter, &list)) {
            exit(4);
        }
    }

    //Creating putters
    for (int i = 0; i < MAX_PUTTERS; i++) {
        if (pthread_create(&putters[i], NULL, thread_putter, &list)) {
            exit(3);
        }
    }


    //Waiting for getters
    for (int i = 0; i < MAX_GETTERS; i++) {
        if (pthread_join(getters[i], NULL)) {
            exit(5);
        }
    }

    //Waiting for putters
    for (int i = 0; i < MAX_PUTTERS; i++) {
        if (pthread_join(putters[i], NULL)) {
            exit(6);
        }
    }
    printf("put_count %d\n get_count %d\n", put_count, get_count);
    assert(put_count == get_count);
    return 0;
}