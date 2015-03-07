#include "tests/lib.h"
int main() {
    int count = 99999;
    while(count > 0) {
        count--;
    }
    int B;
    int *A;
    A = (int*)500;
    B = *A;
    B = B;
    return 0;
}