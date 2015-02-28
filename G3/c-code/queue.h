#define NUM_LIST_IN_HASHTABLE 8;

//parameter for the function get_list. MIN to return a random list with few items
//MAX to return a list with many values
#define MIN 0;  
#define MAX 1;

typedef struct node {
	void *item;
	struct node *next;
} node_t;

typedef struct list {
	node_t	*head;
	node_t	*tail;
} list_t;

typedef struct table {
    list_t table[NUM_LIST_IN_HASHTABLE];
    int num_items;
}
/* table_init initializes a new table */
void table_init(table_t *t);

/* table_put adds the item to the tail of the table */
void table_put(table_t *t, void *item);

/* table_get removes and returns the head of the table in item */
void *table_get(table_t *t);