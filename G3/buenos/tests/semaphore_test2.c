#include "tests/lib.h"

static const char semaphore_name[] = "brian";
static const char process_to_start[] = "[disk]semaphore_test2";
int main() {
    usr_sem_t *semaphore;
    int result;
    semaphore = syscall_sem_open(semaphore_name, -1);
    if (semaphore == NULL) {
        syscall_write(stdout, "Failed to reopen semaphore - Fail\n", 35);
        syscall_halt();
        return 2;
    }
    syscall_write(stdout, "Reopened semaphore - success\n", 29);
    result = syscall_sem_p(semaphore);
    if (result == 0) {
        syscall_write(stdout, "Semaphore blocked - success\n", 29);
    } else {
        syscall_write(stdout, "Semaphore failed to block - Fail\n", 34);
        syscall_halt();
    }

    return 0;
}