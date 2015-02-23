#include "tests/lib.h"

int main(void)
{

    int child1;
    int child2;
    char string[] = "start first process of test1\n";
    printit(string, sizeof(string));
    child1 = syscall_exec("[disk]test1");

    char string2[] = "start second process of test1\n";
    printit(string2, sizeof(string2));

    child2 = syscall_exec("[disk]test1");
    //print("joining test1 as child1");

    char string3[] = "join first instansce of test1\n";
    printit(string3, sizeof(string3));

    syscall_join(child1);
    //print("joining test2 as child2");

    char string4[] = "join second instansce of test1\n";
    printit(string4, sizeof(string4));
    syscall_join(child2);
    //print("exit child1");

    syscall_halt();
    return 0;

}


