#include "tests/lib.h"
int main() {
    int test = syscall_join(syscall_exec("[disk]faul_addr_test"));
    if (test == 0) {
        syscall_write(1, "Returværdi == 0\n\0", 17);
    } else {
        syscall_write(1, "Returværdi != 0\n\0", 17);
    }
    syscall_halt();
    return 0;
}