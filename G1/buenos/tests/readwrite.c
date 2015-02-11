#include "tests/lib.h"
#include "proc/syscall.h"

int main()
{
    int len = 64;
    char c[len];
    syscall_read(FILEHANDLE_STDIN, c, len);
    syscall_write(FILEHANDLE_STDOUT, c, len);

    syscall_halt();

    return 0;
}