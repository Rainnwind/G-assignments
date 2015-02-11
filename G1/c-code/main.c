#include <stdio.h>
#include "heap.c"

void unit_test_heap_initialize()
{
    printf("\nInitilizing unit_test_heap_initialize\n");
    heap heap_max;
    heap_initialize(&heap_max);
    printf("Expected alloc_size: %d, actual alloc_size: %zd - %s\n", INIT_SIZE, heap_max.alloc_size, heap_max.alloc_size == INIT_SIZE ? "Passed" : "Failed");
}

void unit_test_heap_clear()
{
    printf("\nInitilizing unit_test_heap_clear\n");
    heap heap_max;
    heap_initialize(&heap_max);
    heap_clear(&heap_max);
    printf("Expected alloc_size: %d, actual alloc_size: %zd - %s\n", 0, heap_max.alloc_size, heap_max.alloc_size == 0 ? "Passed" : "Failed");
}

void unit_test_heap_size()
{
    printf("\nInitilizing unit_test_heap_size\n");
    heap heap_max;
    printf("Expected size: %d, actual size: %zd - %s\n", -1, heap_size(&heap_max), heap_size(&heap_max) == -1 ? "Passed" : "Failed");
    heap_initialize(&heap_max);

    printf("Expected size: %d, actual size: %zd - %s\n", 0, heap_size(&heap_max), heap_size(&heap_max) == 0 ? "Passed" : "Failed");

    heap_insert(&heap_max, NULL, 5);
    printf("Expected size: %d, actual size: %zd - %s\n", 1, heap_size(&heap_max), heap_size(&heap_max) == 1 ? "Passed" : "Failed");

    heap_insert(&heap_max, NULL, 5);
    printf("Expected size: %d, actual size: %zd - %s\n", 2, heap_size(&heap_max), heap_size(&heap_max) == 2 ? "Passed" : "Failed");
}

void unit_test_heap_top()
{
    node *node;
    printf("\nInitilizing unit_test_heap_top\n");
    heap heap_max;
    node = heap_top(&heap_max);
    printf("Expected NULL pointer - %s\n", node == NULL ? "Passed" : "Failed");
    heap_initialize(&heap_max);
    node = heap_top(&heap_max);
    printf("Expected NULL pointer - %s\n", node == NULL ? "Passed" : "Failed");


    heap_insert(&heap_max, NULL, 2);
    node = heap_top(&heap_max);
    printf("Expected priority: %d, actual priority: %d - %s\n", 2, node->priority, node->priority == 2 ? "Passed" : "Failed");

    heap_insert(&heap_max, NULL, 10);
    node = heap_top(&heap_max);
    printf("Expected priority: %d, actual priority: %d - %s\n", 10, node->priority, node->priority == 10 ? "Passed" : "Failed");

    heap_insert(&heap_max, NULL, 5);
    node = heap_top(&heap_max);
    printf("Expected priority: %d, actual priority: %d - %s\n", 10, node->priority, node->priority == 10 ? "Passed" : "Failed");


    heap_insert(&heap_max, NULL, 30);
    node = heap_top(&heap_max);
    printf("Expected priority: %d, actual priority: %d - %s\n", 30, node->priority, node->priority == 30 ? "Passed" : "Failed");

    heap_insert(&heap_max, NULL, 45);
    node = heap_top(&heap_max);
    printf("Expected priority: %d, actual priority: %d - %s\n", 45, node->priority, node->priority == 45 ? "Passed" : "Failed");

    heap_insert(&heap_max, NULL, 3);
    node = heap_top(&heap_max);
    printf("Expected priority: %d, actual priority: %d - %s\n", 45, node->priority, node->priority == 45 ? "Passed" : "Failed");

    for (int i = 0; i < 2048; i++)
    {
        heap_insert(&heap_max, NULL, i);
    }
    node = heap_top(&heap_max);
    printf("Expected priority: %d, actual priority: %d - %s\n", 2047, node->priority, node->priority == 2047 ? "Passed" : "Failed");
}

void unit_test_heap_insert()
{
    printf("\nInitilizing unit_test_heap_insert\n");
    heap heap_max;
    heap_initialize(&heap_max);

    //Testing that the array is in fact dynamic - Inserting has already been tested with `unit_test_heap_top`

    for (int i = 0; i < 1024; i++)
    {
        heap_insert(&heap_max, NULL, i);
    }
    size_t size = heap_max.alloc_size;
    printf("Expected alloc_size: %d, alloc_size size: %zd - %s\n", INIT_SIZE, size, size == INIT_SIZE ? "Passed" : "Failed");

    heap_insert(&heap_max, NULL, 1024);
    size = heap_max.alloc_size;
    printf("Expected alloc_size: %d, alloc_size size: %zd - %s\n", INIT_SIZE * 2, size, size == INIT_SIZE * 2 ? "Passed" : "Failed");

    for (int i = 0; i < 1023; i++)
    {
        heap_insert(&heap_max, NULL, i);
    }
    size = heap_max.alloc_size;
    printf("Expected alloc_size: %d, alloc_size size: %zd - %s\n", INIT_SIZE * 2, size, size == INIT_SIZE * 2 ? "Passed" : "Failed");

    heap_insert(&heap_max, NULL, 1024);
    size = heap_max.alloc_size;
    printf("Expected alloc_size: %d, alloc_size size: %zd - %s\n", INIT_SIZE * 4, size, size == INIT_SIZE * 4 ? "Passed" : "Failed");
}

void unit_test_heap_pop()
{
    node *node;
    printf("\nInitilizing unit_test_heap_pop\n");
    heap heap_max;
    heap_initialize(&heap_max);

    heap_insert(&heap_max, NULL, 10);
    node = heap_pop(&heap_max);
    printf("Expected priority: %d, actual priority: %d - %s\n", 10, node->priority, node->priority == 10 ? "Passed" : "Failed");

    heap_insert(&heap_max, NULL, 3);
    heap_insert(&heap_max, NULL, 1);
    heap_insert(&heap_max, NULL, 9);
    node = heap_pop(&heap_max);
    printf("Expected priority: %d, actual priority: %d - %s\n", 9, node->priority, node->priority == 9 ? "Passed" : "Failed");


    heap_insert(&heap_max, NULL, 10);
    heap_insert(&heap_max, NULL, 15);
    heap_insert(&heap_max, NULL, 2);
    heap_insert(&heap_max, NULL, 30);
    heap_insert(&heap_max, NULL, 15);
    node = heap_pop(&heap_max);
    printf("Expected priority: %d, actual priority: %d - %s\n", 30, node->priority, node->priority == 30 ? "Passed" : "Failed");

    node = heap_pop(&heap_max);
    printf("Expected priority: %d, actual priority: %d - %s\n", 15, node->priority, node->priority == 15 ? "Passed" : "Failed");

    node = heap_pop(&heap_max);
    printf("Expected priority: %d, actual priority: %d - %s\n", 15, node->priority, node->priority == 15 ? "Passed" : "Failed");

    node = heap_pop(&heap_max);
    printf("Expected priority: %d, actual priority: %d - %s\n", 10, node->priority, node->priority == 10 ? "Passed" : "Failed");

    node = heap_pop(&heap_max);
    printf("Expected priority: %d, actual priority: %d - %s\n", 3, node->priority, node->priority == 3 ? "Passed" : "Failed");

    node = heap_pop(&heap_max);
    printf("Expected priority: %d, actual priority: %d - %s\n", 2, node->priority, node->priority == 2 ? "Passed" : "Failed");

    node = heap_pop(&heap_max);
    printf("Expected priority: %d, actual priority: %d - %s\n", 1, node->priority, node->priority == 1 ? "Passed" : "Failed");

    node = heap_pop(&heap_max);
    printf("Expected NULL pointer - %s\n", node == NULL ? "Passed" : "Failed");
}

int main()
{
    unit_test_heap_initialize();
    unit_test_heap_clear();
    unit_test_heap_size();
    unit_test_heap_top();
    unit_test_heap_insert();
    unit_test_heap_pop();

    return 0;
}