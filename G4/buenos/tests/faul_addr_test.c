#include "tests/lib.h"
int main() {
    int *A;
    int B;
    A = (int*)500;
    B = *A;
    B = B;
    syscall_write(1, "Made it this far...\n\0", 21);
    return 0;
}