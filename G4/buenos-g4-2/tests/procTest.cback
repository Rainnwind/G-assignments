#include "tests/lib.h"

int main(void)
{
    int child1;
    int child2;
    char string[] = "start first process of test1\n";
    syscall_write(stdout, string, sizeof(string));
    child1 = syscall_exec("[disk]test1");

    char string2[] = "join first instance of test1\n";
    syscall_write(stdout, string2, sizeof(string2));

    syscall_join(child1);

    char string3[] = "start second process of test1\n";
    syscall_write(stdout, string3, sizeof(string3));

    child2 = syscall_exec("[disk]test1");

    char string4[] = "join second instance of test1\n";
    syscall_write(stdout, string4, sizeof(string4));
    syscall_join(child2);

    syscall_halt();
    return 0;

}


