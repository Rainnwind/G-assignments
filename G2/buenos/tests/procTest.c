#include "tests/lib.h"

//void print(char* string);
int main(void)
{

    int child1;
    int child2;
    //print("234567890");
    child1 = syscall_exec("[disk]test1");
    //print("starting process test2");
    child2 = syscall_exec("[disk]test2");
    //print("joining test1 as child1");
    syscall_join(child1);
    //print("joining test2 as child2");
    syscall_join(child2);
    //print("exit child1");
    syscall_exit(child1);
    //print("exit child2");
    syscall_exit(child2);
    //print("calling syscal halt");

    syscall_halt();
    return 0;

}

/*void print(char *string){
        syscall_write(1, "s", 1);
        syscall_write(1, "s", 1);
        syscall_write(1, "s", 1);

        syscall_write(1, string, sizeof(string));
    }

*/
