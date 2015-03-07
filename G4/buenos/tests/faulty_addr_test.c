#include "tests/lib.h"
int main() {
    int B;
    int *A;
    A = (int*)500;
    B = *A;
    B = B;
    return 0;
}