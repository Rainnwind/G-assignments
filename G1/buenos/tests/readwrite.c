#include "tests/lib.h"
#include "proc/syscall.h"

void unit_test_read()
{
    syscall_write(FILEHANDLE_STDOUT, "Initializing unit_test_read\n", 28);

    int length;
    char c[64];

    syscall_write(FILEHANDLE_STDOUT, "Please type in your characters: ", 32);
    length = syscall_read(FILEHANDLE_STDIN, c, 64);
    syscall_write(FILEHANDLE_STDOUT, "\nexpecting max length 64 and min length 1", 41);
    if (length >= 1 || length <= 64)    //Testing that the read length is between 1 and 64. It cannot be 0 since our implementation reserves one byte for
                                        //null terminating string
    {
        syscall_write(FILEHANDLE_STDOUT, " - Passed\n", 10);
    }
    else if(length == 0 || length > 64)
    {
        syscall_write(FILEHANDLE_STDOUT, " - Failed\n", 10);
    }
    else
    {
        syscall_write(FILEHANDLE_STDOUT, " - Errorcode -1 received\n", 25);
    }

    char cc[128];
    syscall_write(FILEHANDLE_STDOUT, "Please type in your characters: ", 32);
    length = syscall_read(FILEHANDLE_STDIN, cc, 128);
    syscall_write(FILEHANDLE_STDOUT, "\nexpecting max length 128 and min length 1", 42);
    if (length >= 1 || length <= 128)   //Testing that the read length is between 1 and 128. It cannot be 0 since our implementation reserves one byte for
                                        //null terminating string
    {
        syscall_write(FILEHANDLE_STDOUT, " - Passed\n", 10);
    }
    else if (length == 0 || length > 128)
    {
        syscall_write(FILEHANDLE_STDOUT, " - Failed\n", 10);
    }
    else
    {
        syscall_write(FILEHANDLE_STDOUT, " - Errorcode -1 received\n", 25);
    }
}

void unit_test_write()
{
    syscall_write(FILEHANDLE_STDOUT, "Initializing unit_test_write\n", 29);
    int length;

    length = syscall_write(FILEHANDLE_STDOUT, "string with 32 characters in it\n", 32);
    if (length == 32) //Testing that the length written is exactly 32 bytes since 32 characters are written
    {
        syscall_write(FILEHANDLE_STDOUT, "Expected length 32, actual length 32 - Passed\n", 46);
    }
    else
    {
        syscall_write(FILEHANDLE_STDOUT, " - Failed\n", 10);
    }


    length = syscall_write(FILEHANDLE_STDOUT, "string with more than 32 characters in it - gotta pass too\n", 59);
    if (length == 59) //Testing that the length written is exactly 59 bytes since 59 characters are written
    {
        syscall_write(FILEHANDLE_STDOUT, "Expected length 59, actual length 59 - Passed\n", 46);
    }
    else
    {
        syscall_write(FILEHANDLE_STDOUT, " - Failed\n", 10);
    }
}

void unit_test_read_write()
{
    syscall_write(FILEHANDLE_STDOUT, "Initializing unit_test_read_write\n", 34);
    int read_length;
    int written_length;

    syscall_write(FILEHANDLE_STDOUT, "Whatever read from terminal is printed in terminal\n", 51);

    char c[64];
    read_length = syscall_read(FILEHANDLE_STDIN, c, 64);
    written_length = syscall_write(FILEHANDLE_STDOUT, c, 64);

    if (read_length == written_length)  //Testing that whatever is read has the same length as what is written since the written part is the char variable
                                        //used to read input
    {
        syscall_write(FILEHANDLE_STDOUT, "read_length == written_length - Passed\n", 39);
    }
    else
    {
        syscall_write(FILEHANDLE_STDOUT, "read_length != written_length - Failed\n", 39);
    }
}

int main()
{
    unit_test_read();
    unit_test_write();
    unit_test_read_write();

    syscall_halt();
    return 0;
}