#include <stdlib.h>
#include "heap.h"
/*
    Allocating room for heap array and initiates root size
*/
void heap_initialize(heap *h)
{
    node *tmp_root;

    h->alloc_size = INIT_SIZE;
    h->initilized = 1;
    tmp_root = malloc(sizeof(node) * h->alloc_size);
    if (tmp_root != NULL)
    {
        h->root = tmp_root;
        h->size = 0;
    }
    else
    {
        exit(1);
    }
}

/*
    If the heap has been initialized the heap is freed from memory
*/
void heap_clear(heap *h)
{
    if (h->initilized == 1)
    {
        h->alloc_size = 0;
        h->size = 0;
        h->initilized = 0;
        free(h->root);
    }
}

/*
    Return the size of the heap.
    If the heap has not been initialized -1 is returned
*/
size_t heap_size(heap *h)
{
    if (h->initilized == 1)
    {
        return h->size;
    }
    else
    {
        return -1;
    }
}

/*
    Returns the node with highest priority.
    If the heap has not been initialized or the heap is empty NULL is returned
*/
void *heap_top(heap *h)
{
    if (h->initilized == 1 && h->size > 0)
    {
        return &h->root[0];
    }
    else
    {
        return NULL;
    }
}

/*
    Insert a new node into the heap - Once inserted the heap is sorted based on the new element.
    The root array is expanded with current size times two, if the array is full.
*/
void heap_insert(heap *h, void *item, int priority)
{
    int p,
        child;
    node *tmp_root;
    if (h->initilized == 1)
    {
        //Expanding size if the array is out of space
        if (h->size >= h->alloc_size)
        {
            h->alloc_size *= 2;
            tmp_root = realloc(h->root, sizeof(node) * h->alloc_size);
            if (tmp_root != NULL)
            {
                h->root = tmp_root;
            }
            else
            {
                exit(2);
            }
        }
        //Inserting new element
        h->root[h->size].thing = item;
        h->root[h->size].priority = priority;

        //Moving newly inserted node up the heap if it needs to.
        p = parrent(h->size);
        child = h->size;

        //Swapping child with parrent as long as parrent priority is lower than child priority
        while (h->root[p].priority < h->root[child].priority && p > -1)
        {
            node tmp_parrent = h->root[p];
            h->root[p] = h->root[child];
            h->root[child] = tmp_parrent;

            child = p;
            p = parrent(child);
        }
        //Updating array with size +1
        h->size++;
    }
    else
    {
        exit(3);
    }
}

/*
    Removes and returns the highest priority - It then sort the remaining heap
    If the heap is empty on function call NULL is returned
*/
void *heap_pop(heap *h)
{
    int p,
        left_child,
        right_child,
        swapwith,
        p_priority,
        l_priority,
        r_priority;
    node tmp_node;
    void *ret_node;

    if (h->initilized == 1 && h->size > 0)
    {
        //Storing return value for later use
        tmp_node = h->root[0];
        //Allocating space for the return node - It's up to the caller to free the return variable.
        //Allocation has been done in order to avoid returning a local variable
        ret_node = (node *)malloc(sizeof(node));
        ret_node = &tmp_node;

        //Swapping top node with last node
        h->root[0] = h->root[h->size - 1];

        //Removing entry of the last element since it's no longed needed
        h->size--;

        //Swapping left child or right_child with parrent as long as one
        //of either child is bigger than their parrent
        p = 0;
        left_child = lchild(p);
        right_child = rchild(p);

        //Loops unltil there are no more left children
        while (left_child < h->size)
        {
            p_priority = h->root[p].priority;
            l_priority = h->root[left_child].priority;
            r_priority = h->root[right_child].priority;

            swapwith = 0;
            //If either left or right child is bigger than parrent than the biggest priority of the childen
            //is being swapped with
            if (right_child < h->size && (l_priority > p_priority || r_priority > p_priority))
            {
                if (l_priority > r_priority)
                {
                    swapwith = left_child;
                }
                else
                {
                    swapwith = right_child;
                }
            }
            else if (l_priority > p_priority)
            {
                swapwith = left_child;
            }

            if (swapwith > 0)
            {
                node tmp_parrent = h->root[p];
                h->root[p] = h->root[swapwith];
                h->root[swapwith] = tmp_parrent;

                p = swapwith;
                left_child = lchild(p);
                right_child = rchild(p);
            }
            else
            {
                break;
            }
        }
        return ret_node;
    }
    else
    {
        return NULL;
    }
}