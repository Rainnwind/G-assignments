#include <stddef.h> // size_t

#define INIT_SIZE 1024

/*
  Macro functions which defines the
    parrent,
    left child and
    right child of a current node

    The heap is 0 indexed.
*/
#define parrent(n) (n - 1) / 2
#define lchild(n) (n * 2) + 1
#define rchild(n) (n * 2) + 2

typedef struct
{
    void *thing;
    int priority;
} node;

typedef struct
{
    node *root;
    size_t size;
    /* You may want to allocate more
       space than strictly needed. */
    size_t alloc_size;
    /*
        initilized is added to have an indication if the
        root array has been initialized.
    */
    char initilized;
} heap;

void heap_initialize(heap *);
void heap_clear(heap *);
size_t heap_size(heap *);
void *heap_top(heap *);
void heap_insert(heap *, void *, int);
void *heap_pop(heap *);