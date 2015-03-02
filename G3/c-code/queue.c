#include <pthread.h>
#include <stdbool.h> 
#include <assert.h>
#include <error.h>
#include <stdio.h>
#include <stdlib.h>
#include "queue.h"

void table_init(table_t *t) {
	for(int i = 0; i < NUM_LIST_IN_TABLE; i++){
		node_t *new = malloc(sizeof(node_t));
		if (new == NULL) {
			perror("malloc failed");
			exit(EXIT_FAILURE);
		}

		new->next = NULL;
		new->item = 0;
		list_t *newlist = malloc(sizeof(list_t));
		newlist->head = newlist->tail = new;
		*(newlist->count) = 0;
        pthread_mutex_init(&(*newlist).llock, NULL);
		
		(*t).table[i] = newlist;
		
	}
}

void table_put(table_t *t, void *item) {
	node_t *new = malloc(sizeof(node_t));
	if (new == NULL) {
		perror("malloc failed");
		exit(EXIT_FAILURE);
	}

	new->item = item;
	new->next = NULL;

	// get a pointer to a list in were a lock
	// has been aquired
	list_t *list = get_list(t, PUT);

	list->tail->next = new;
	list->tail = new;
	*(list->count) = *(list->count) + 1;

	/*realease loock*/
	int retval = pthread_mutex_unlock(&(list->llock));
	assert(retval == 0);
}

void *table_get(table_t *t) {

	// return the index of a list in the table to get an
	// intem from and lock the list.
	list_t list = get_list(t, GET);
	if(list == null){
		return 0;
	}
	node_t *old = t[list]->head;

	void *item = old->next->item;

	/* update the head and free the old memory */
	t[list]->head = old->next;
	free(old);
	/*realease loock*/
	int retval = pthread_mutex_unlock(&(t->table[list]->lock));
	assert(retval == 0);
		
	return item;
}
list_t *get_list(table_t *t, int mode){
	case mode

	case 0;
 		//return index of a random chosen list in the table
		int i = 0;
		do{
    		rand((unsigned)time(NULL));
    		int rand_table = random_int(NUM_LIST_IN_TABLE);
    		int retval = pthread_mutex_lock(&(t->table[rand_table]->llock);
  		} while (!(retval == 0 && i++ < NUM_LIST_IN_TABLE); 
		return t.table[rand_table];


	case 1;
		//Itereate the lists in the table to find a nonempty list
		//return -1 if there are no more items to get.
		for (int i = 0; i < count; ++i)
		{
			//if found and a lock could be aquired return the index of the list
			if(*(t->table[i]->head->item) > 0 && pthread_mutex_unlock(&(t->table[i]->llock) == 0) {
				return t.table[i]; 
			}
		}
		return null;		

}
int main(int argc, char **argv) {
	queue_t q;

	queue_init(&q);

	int val = 42;

	queue_put(&q, &val);
	queue_put(&q, &val);

	assert(42 == *(int *)queue_get(&q));
	assert(42 == *(int *)queue_get(&q));
	assert(NULL == queue_get(&q));
	assert(NULL == queue_get(&q));

	return 0;
}
