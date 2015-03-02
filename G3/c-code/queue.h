#include <pthread.h>
#include <stdlib.h>
#define NUM_LIST_IN_TABLE 8

//parameter for the function get_list. MIN to return a random list with few items
//MAX to return a list with many values
#define PUT 0
#define GET 1

typedef struct node {
	void *item;
	struct node *next;
} node_t;

typedef struct list {
	pthread_mutex_t llock;
    int count;
	node_t	*head;
	node_t	*tail;
	struct list *next;
} list_t;

// table of lists
typedef struct table {
  list_t *head; 
  list_t *tail;
  bool exit;
} table_t;

//generate a random number in the range 0 to NUM_LIST_IN_TABLE
int random_int(int max) {
    srand((unsigned)time(NULL));
    return (rand() % max);
}
/* table_init initializes a new table */
void table_init(table_t *t);

/* table_put adds the item to the tail of the table */
void table_put(table_t *t, void *item);

/* table_get removes and returns the head of the table in item */
void *table_get(table_t *t);

/* get_list return index of a list tu put or get an item from*/
list_t *get_list(table_t *t, int mode);