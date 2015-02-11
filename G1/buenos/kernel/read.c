#include "lib/debug.h"
#include "drivers/gcd.h"
#include "kernel/read.h"
#include "lib/libc.h"
#include "proc/syscall.h"

int syscall_read(uint32_t f_handle, uint32_t buffer_addr, uint32_t length)
{
    //Handling console read
    if (f_handle == FILEHANDLE_STDIN)
    {
        DEBUG("debugreadwrite", "Starting syscall_read\n\n\n\n\n");
        char *input_buffer = (char *)buffer_addr;

        int max_read = length;


        device_t *dev;
        gcd_t *gcd;
        char tempbuffer[max_read];

        int len;
        int read_len = 0;

        /* Find system console (first tty) */
        dev = device_get(YAMS_TYPECODE_TTY, 0);
        if (dev == NULL)
        {
            return -1;
        }
        gcd = (gcd_t *)dev->generic_device;
        if (gcd == NULL)
        {
            return -1;
        }

        //Reading until the buffer is full or the user presses enter(key 13) in the console
        do
        {
            //Reading one character at a time
            len = gcd->read(gcd, tempbuffer, 1);
            //Writing read character to console - Gives a feeling of interactiveness
            gcd->write(gcd, tempbuffer, len);
            //Copying read character into users buffer
            input_buffer[read_len] = tempbuffer[0];
            //Incrementing total read length
            read_len += len;
        }
        while (tempbuffer[0] != 13 && read_len <= (max_read - 1));
        //Last position of input_buffer is reserved for the null-terminating string
        input_buffer[read_len] = '\0';

        DEBUG("debugreadwrite", "\n---Read from terminal: %s\n", input_buffer);
        DEBUG("debugreadwrite", "Read bytes: %d\n", read_len);
        DEBUG("debugreadwrite", "\n\nEnding syscall_read\n\n\n\n\n");
        return read_len;
    }
    //Handling file read
    else
    {
        return -1;
    }
}