#include <pthread.h>
#include <stdbool.h> 
#include <assert.h>
#include <error.h>
#include <stdio.h>
#include <stdlib.h>
#include "queue.h"

void table_init(table_t *t) {
		
	table_t *newtable = malloc(sizeof(table_t));
	newtable->head = newtable->tail = NULL;
	
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
        if (newtable->head == NULL)
        {
           	newtable->head = newlist;
           	newtable->tail = newlist;
           	newtable->tail->next = newlist;
		}
		else
		{
			newtable->tail->next = newlist;
			newtable->tail = newlist;
		}
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
	list_t *list = malloc(sizeof(list));
	list = get_list(t, PUT);  
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
	
	list_t *list = malloc(sizeof(list_t));	
	list = t->tail;
	
    if(mode == 0){ //mode put
    	//return pointer to a random chosen list in the table
    	int retval = -1;
		do{
	    	int rand_list = random_int(NUM_LIST_IN_TABLE);
	    	for(int j = 0; j < rand_list; j++){
	    		list = list->next;
	    	}
	    	retval = pthread_mutex_lock(&(*list).llock);
	  	} while (!(retval == 0)); 
		return list;
	}
	else { //mode get
	    //Itereate the lists in the table to find a nonempty list:
		//set table.exit to true if there are no more items.
		for (int i = 0; i < NUM_LIST_IN_TABLE; ++i)
		{
			int count = list->count;
			//if found and a lock could be aquired return the index of the list
			if( count > 0 && pthread_mutex_unlock(&(list->llock)) == 0) {
				return list;
			}
			list = list->next;
		}
		t->exit = true;
	}
	return list; //not used, but we need to return a list		
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
