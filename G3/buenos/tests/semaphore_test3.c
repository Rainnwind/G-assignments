#include "tests/lib.h"
#define write(s) syscall_write(1, s, sizeof(s))
void write_char(char c) {
    syscall_write(1, &c, 1);
}
void write_int(int n) {
    int k, leading_zeros, div;

    if (n == 0) {
        write_char('0');
        return;
    }

    if (n < 0) {
        write("-");
        n *= -1;
    }

    k = 1000000000;
    leading_zeros = 1;
    while (k > 0) {
        div = n / k;
        if (leading_zeros && div != 0) {
            leading_zeros = 0;
        }
        if (!leading_zeros) {
            write_char(div + '0');
        }
        n %= k;
        k /= 10;
    }
}

static const char semaphore_name[] = "brian";
int main() {
    usr_sem_t *semaphore;
    semaphore = syscall_sem_open(semaphore_name, -1);
    if (semaphore == NULL) {
        syscall_write(stdout, "Failed to reopen semaphore - Fail\n", 35);
        syscall_halt();
    }
    syscall_write(stdout, "Reopened semaphore - success\n", 29);
    int result = -1;

    int sem_v_count = 0;
    //Loop continues untill all bound by the semaphore are free
    while (result != 0) {
        int sleep_count = 99999;
        while(sleep_count > 0) {
            sleep_count--;
        }
        result = syscall_sem_destroy(semaphore);
        if (result != 0 && sem_v_count < 15) {
            syscall_write(stdout, "Could not close semaphore within boundaries - count: ", 53);
            write_int(sem_v_count);
            syscall_write(stdout, " < 15 - success", 15);
            syscall_write(stdout, "\n", 1);
        } else if (result != 0) {
            syscall_write(stdout, "Could not close semaphore outside boundaries - count: ", 54);
            write_int(sem_v_count);
            syscall_write(stdout, " < 15 - Fail", 12);
            syscall_write(stdout, "\n", 1);
        syscall_halt();
        } else {
            syscall_write(stdout, "Closed semaphore outside boundaries - count: ", 45);
            write_int(sem_v_count);
            syscall_write(stdout, " < 15 - success", 15);
            syscall_write(stdout, "\n", 1);
        }

        syscall_sem_v(semaphore);

        sem_v_count++;

    }
    return 0;
}