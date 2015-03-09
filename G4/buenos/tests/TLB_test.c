#include "tests/lib.h"
int main() {

    int test = syscall_join(syscall_exec("[disk]faul_addr_test"));
    if (test == 0) {
        syscall_write(1, "Returværdi == 0 - Fail\n\0", 24);
    } else if (test == -1) {
        syscall_write(1, "Returværdi == -1 - Success\n\0", 30);
    } else {
        syscall_write(1, "Returværdi != 0 and != -1 - Fail\n\0", 34);
    }

    int test2 = syscall_join(syscall_exec("[disk]faul_addr_test"));
    if (test2 == 0) {
        syscall_write(1, "Returværdi == 0 - Fail\n\0", 24);
    } else if (test2 == -1) {
        syscall_write(1, "Returværdi == -1 - Success\n\0", 30);
    } else {
        syscall_write(1, "Returværdi != 0 and != -1 - Fail\n\0", 34);
    }

    int test3 = syscall_join(syscall_exec("[disk]faul_addr_test"));
    if (test3 == 0) {
        syscall_write(1, "Returværdi == 0 - Fail\n\0", 24);
    } else if (test3 == -1) {
        syscall_write(1, "Returværdi == -1 - Success\n\0", 30);
    } else {
        syscall_write(1, "Returværdi != 0 and != -1 - Fail\n\0", 34);
    }

    int test4 = syscall_join(syscall_exec("[disk]faul_addr_test"));
    if (test4 == 0) {
        syscall_write(1, "Returværdi == 0 - Fail\n\0", 24);
    } else if (test4 == -1) {
        syscall_write(1, "Returværdi == -1 - Success\n\0", 30);
    } else {
        syscall_write(1, "Returværdi != 0 and != -1 - Fail\n\0", 34);
    }

    int test5 = syscall_join(syscall_exec("[disk]no_fault_addr"));
    if (test5 == 0) {
        syscall_write(1, "Returværdi == 0 - Success\n\0", 27);
    } else if (test5 == -1) {
        syscall_write(1, "Returværdi == -1 - Fail\n\0", 25);
    } else {
        syscall_write(1, "Returværdi != 0 and != -1 - Fail\n\0", 34);
    }

    syscall_halt();
    return 0;
}