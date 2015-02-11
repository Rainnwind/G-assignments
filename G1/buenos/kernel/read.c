#include "lib/debug.h"
#include "drivers/gcd.h"
#include "kernel/read.h"
#include "lib/libc.h"
#include "kernel/assert.h"

int read_tty(uint32_t f_handle, uint32_t buffer_addr, uint32_t length)
{
    //Since f_handle is never used - That is, no pointer to a file is needed since the
    //this is a simplified version of read which reads from the terminal rather than a file.
    //f_handle = f_handle is only to avoid errors telling us that it is unused
    f_handle = f_handle;


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

    //Reading until the buffer is full or the user presses enter in the console
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
    while ((stringcmp(tempbuffer, "\r")) != 0 && read_len <= (max_read - 1));
    //Last position of input_buffer is reserved for the null-terminating string
    input_buffer[read_len] = '\0';

    DEBUG("debuginit", "\n---Read from terminal: %s\n", input_buffer);
    DEBUG("debuginit", "Read bytes: %d\n", read_len);
    return read_len;
}