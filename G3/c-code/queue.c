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
		newlist->count = 0;
        pthread_mutex_init(&(*newlist).llock, NULL);
		
		(*t).table[i] = newlist;
		t->exit = false;
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
	list->count = list->count + 1;

	/*realease loock*/
	int retval = pthread_mutex_unlock(&(list->llock));
	assert(retval == 0);
}

void *table_get(table_t *t) {

	// return the index of a list in the table to get an
	// item from and lock the list.
	list_t *list = (get_list(t, GET));

	node_t *old = list->head;

	void *item = old->next->item;

	/* update the head and free the old memory */
	list->head = old->next;
	list->count = list->count - 1;
	
	/*realease loock*/
	int retval = pthread_mutex_unlock(&(list->llock));
	free(old);
	assert(retval == 0);
	return item;
}
list_t *get_list(table_t *t, int mode){
	int retval = -1;
	int i = 0;
	int rand_table;
    if(mode == 0){ //mode put
 		//return index of a random chosen list in the table
		do{
	    	rand_table = random_int(NUM_LIST_IN_TABLE);
	    	retval = pthread_mutex_lock(&(t->table[rand_table]->llock));
	  	} while (!(retval == 0 && i++ < NUM_LIST_IN_TABLE)); 
		return t->table[rand_table];
	}
	else { //mode get
	    //Itereate the lists in the table to find a nonempty list:
		//set table.exit to true if there are no more items.
		for (int i = 0; i < NUM_LIST_IN_TABLE; ++i)
		{
			list_t *list = t->table[i];
			int count = (int)*(list->count);
			//if found and a lock could be aquired return the index of the list
			if( count > 0 && pthread_mutex_unlock(&(t->table[i]->llock)) == 0) {
				return t->table[i];
			}
		}
		t->exit = true;
	}
	return t->table[i]; //not used, but we need to return some list		
}
int main(int argc, char* argv[]) {
	printf("1111111111111");
	table_t *table = malloc(sizeof(table_t));
	if (table == NULL) {
		perror("malloc failed");
		exit(EXIT_FAILURE);
	}
	
	table_init(table);
	printf("222222222");
	int val = 42;

	table_put(table, &val);
	table_put(table, &val);

	assert(42 == *(int *)table_get(table));
	assert(42 == *(int *)table_get(table));
	assert(NULL == table_get(table));
	assert(NULL == table_get(table));

	return 0;
}
