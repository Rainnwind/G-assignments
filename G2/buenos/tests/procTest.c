#include "tests/lib.h"

void print(char* string);
int main(void)
{

    int child1;
    int child2;
    print("234567890");
    child1 = syscall_exec("[disk]test1");
    //print("starting process test2");
    child2 = syscall_exec("[disk]test1");
    //print("joining test1 as child1");
    syscall_join(child1);
    //print("joining test2 as child2");
    syscall_join(child2);
    //print("exit child1");

    syscall_halt();
    return 0;

}

void print(char* string){
    unsigned int i;
    for(i = 0; i < sizeof(string); i++) {
        syscall_write(1, string[i], 1);
    }
}

