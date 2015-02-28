#include "tests/lib.h"

// Only works for constants and arrays, so watch out.
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
static const char process_to_start[] = "[disk]semaphore_test2";
static const char second_process_to_start[] = "[disk]semaphore_test3";
int main() {
    int i = 0;
    for (i = 0; i < 50; i++) {
        syscall_write(stdout, "\n", 1);
    }
    int process_ids[15];
    usr_sem_t *semaphore;
    int result;
    int wait_for_it;
    semaphore = syscall_sem_open(semaphore_name, 0);
    if (semaphore == NULL) {
        syscall_write(stdout, "Failed to create a semaphore\n", 29);
        syscall_halt();
    }

    //Spawning 15 process to all use the semaphore on syscall_sem_p

    for (i = 0; i < 15; i++) {
        process_ids[i] = syscall_exec(process_to_start);
    }

    //Making sure that every process above has initiated
    wait_for_it = syscall_exec(second_process_to_start);
    //Waiting for every process to initiate
    syscall_join(wait_for_it);

    char test_open_semaphore[129];
    int name_count = 0;
    for (name_count = 0; name_count < 129; name_count++) {
        test_open_semaphore[name_count] = 'a';
        semaphore = syscall_sem_open(test_open_semaphore, 0);
        if (semaphore == NULL && name_count >= 128) {
            syscall_write(stdout, "Could not create a semaphore outside boundaries - count: ", 57);
            write_int(name_count);
            syscall_write(stdout, " >= 128 - success", 17);
            syscall_write(stdout, "\n", 1);
        } else if (semaphore == NULL) {
            syscall_write(stdout, "Failed to create a semaphore witnin boundaries - count: ", 56);
            write_int(name_count);
            syscall_write(stdout, " < 128 - Fail", 13);
            syscall_write(stdout, "\n", 1);
            syscall_halt();
        } else {
            syscall_write(stdout, "Semaphore created within boundaries - count: ", 45);
            write_int(name_count);
            syscall_write(stdout, " < 128 - success", 16);
            syscall_write(stdout, "\n", 1);
        }
    }

    char test_close_semaphore[129];
    for(name_count = 0; name_count < 129; name_count++) {
        test_close_semaphore[name_count] = 'a';
        semaphore = syscall_sem_open(test_close_semaphore, -1);
        if (semaphore == NULL && name_count >= 128) {
            syscall_write(stdout, "Could not reopen the semaphore outside boundaries - count: ", 59);
            write_int(name_count);
            syscall_write(stdout, " >= 128 - success", 17);
            syscall_write(stdout, "\n: ", 1);
        } else if (semaphore == NULL) {
            syscall_write(stdout, "Failed to reopen the semaphore witnin boundaries - count: ", 58);
            write_int(name_count);
            syscall_write(stdout, " < 128 - Fail", 13);
            syscall_write(stdout, "\n", 1);
            syscall_halt();
        } else {
            syscall_write(stdout, "Semaphore reopened within boundaries - count: ", 46);
            write_int(name_count);
            syscall_write(stdout, " < 128 - success", 16);
            syscall_write(stdout, "\n", 1);

            //Testing that the reopened semaphore will actually close
            int result = syscall_sem_destroy(semaphore);
            if (result != 0) {
                syscall_write(stdout, "Failed to close the semaphore witnin boundaries - count: ", 57);
                write_int(name_count);
                syscall_write(stdout, " < 128 - Fail", 13);
                syscall_write(stdout, "\n", 1);
                syscall_halt();
            } else {
                syscall_write(stdout, "Semaphore closed within boundaries - count: ", 44);
                write_int(name_count);
                syscall_write(stdout, " < 128 - success", 16);
                syscall_write(stdout, "\n", 1);
            }
        }
    }

    char test_open_again_semaphore[129];
    name_count = 0;
    for (name_count = 0; name_count < 129; name_count++) {
        test_open_again_semaphore[name_count] = 'a';
        semaphore = syscall_sem_open(test_open_again_semaphore, 0);
        if (semaphore == NULL && name_count >= 128) {
            syscall_write(stdout, "Could not create a semaphore outside boundaries - count: ", 57);
            write_int(name_count);
            syscall_write(stdout, " >= 128 - success", 17);
            syscall_write(stdout, "\n", 1);
        } else if (semaphore == NULL) {
            syscall_write(stdout, "Failed to create a semaphore witnin boundaries - count: ", 56);
            write_int(name_count);
            syscall_write(stdout, " < 128 - Fail", 13);
            syscall_write(stdout, "\n", 1);
            syscall_halt();
        } else {
            syscall_write(stdout, "Semaphore created within boundaries - count: ", 45);
            write_int(name_count);
            syscall_write(stdout, " < 128 - success", 16);
            syscall_write(stdout, "\n", 1);
        }
    }

    char test_close_again_semaphore[129];
    for(name_count = 0; name_count < 129; name_count++) {
        test_close_again_semaphore[name_count] = 'a';
        semaphore = syscall_sem_open(test_close_again_semaphore, -1);
        if (semaphore == NULL && name_count >= 128) {
            syscall_write(stdout, "Could not reopen the semaphore outside boundaries - count: ", 59);
            write_int(name_count);
            syscall_write(stdout, " >= 128 - success", 17);
            syscall_write(stdout, "\n: ", 1);
        } else if (semaphore == NULL) {
            syscall_write(stdout, "Failed to reopen the semaphore witnin boundaries - count: ", 58);
            write_int(name_count);
            syscall_write(stdout, " < 128 - Fail", 13);
            syscall_write(stdout, "\n", 1);
            syscall_halt();
        } else {
            syscall_write(stdout, "Semaphore reopened within boundaries - count: ", 46);
            write_int(name_count);
            syscall_write(stdout, " < 128 - success", 16);
            syscall_write(stdout, "\n", 1);

            //Testing that the reopened semaphore will actually close
            int result = syscall_sem_destroy(semaphore);
            if (result != 0) {
                syscall_write(stdout, "Failed to close the semaphore witnin boundaries - count: ", 57);
                write_int(name_count);
                syscall_write(stdout, " < 128 - Fail", 13);
                syscall_write(stdout, "\n", 1);
                syscall_halt();
            } else {
                syscall_write(stdout, "Semaphore closed within boundaries - count: ", 44);
                write_int(name_count);
                syscall_write(stdout, " < 128 - success", 16);
                syscall_write(stdout, "\n", 1);
            }
        }
    }


    syscall_write(stdout, "\n", 1);
    syscall_write(stdout, "\n", 1);
    syscall_write(stdout, "\n", 1);
    //Ensuring that every process has terminated
    for (i = 0; i < 15; i++) {
        syscall_join(process_ids[i]);
    }
    syscall_halt();
    return 0;
}
