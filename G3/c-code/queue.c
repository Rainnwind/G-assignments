#include <assert.h>
#include <error.h>
#include <stdio.h>
#include <stdlib.h>

#include "queue.h"

void table_init(table_t *t) {
	node_t *new = malloc(sizeof(node_t));
	if (new == NULL) {
		perror("malloc failed");
		exit(EXIT_FAILURE);
	}

	new->next = NULL;
	new->item = 0;
	for(int i = 0; i < NUM_LIST_IN_TABLE){
		t[i]->head = t[i]->tail = new;
	}
	t->num_items = 0;
}

void table_put(table_t *t, void *item) {
	node_t *new = malloc(sizeof(node_t));
	if (new == NULL) {
		perror("malloc failed");
		exit(EXIT_FAILURE);
	}

	new->item = item;
	new->next = NULL;

	/* add the new node to the tail */
	int min_list = get_list(t, MIN)


/*mutex lock on list*/
	

	t[min_list]->tail->next = new;
	t[min_list]->tail = new;
	t->num_items++;
}

void *table_get(table_t *t) {
	if((t->num_items) == 0) {
		return -1;
	}
	int max_list = get_list(t, MAX)
	node_t *old = t[max_list]->head;

	/* note that the head contains a 'dummy' node. That's why we test
	 * old->next. */
	if (old->next == NULL) {
		return NULL; /* queue was empty */
	}

	void *item = old->next->item;

	/* update the head and free the old memory */
	q->head = old->next;
	free(old);

	return item;
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
