#include "tests/lib.h"
#include "proc/syscall.h"

int main()
{
    int len = 64;
    char c[len];
    syscall_read(FILEHANDLE_STDIN, c, len);
    syscall_write(FILEHANDLE_STDOUT, c, len);

    int length = 64;
    syscall_write(FILEHANDLE_STDOUT, ">>>hej med dig<<<\0lol", length);

    syscall_halt();

    return 0;
}