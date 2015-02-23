#include "tests/lib.h"

void printit(char *string, int l) {
    int i = 0;
    while (i < l) {
        syscall_write(1, &string[i], 1);
        i++;
    }
}
int main(void)
{

    int child1;
    int child2;
    printit("234567890", 9);
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


