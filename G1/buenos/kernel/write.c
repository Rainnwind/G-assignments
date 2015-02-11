#include "lib/debug.h"
#include "drivers/gcd.h"
#include "kernel/write.h"
#include "kernel/assert.h"
#include "lib/libc.h"


int write_tty(uint32_t f_handle, uint32_t buffer_addr, uint32_t length)
{
    DEBUG("debuginit", "Starting write_tty\n\n\n\n\n");
    //Since f_handle is never used - That is, no pointer to a file is needed since the
    //this is a simplified version of read which reads from the terminal rather than a file.
    //f_handle = f_handle is only to avoid errors telling us that it is unused
    f_handle = f_handle;

    //Collecting the start of the buffer - the size of it is contained by `length`
    char *cc = (char *)buffer_addr;

    int len;
    int written_len = 0;

    //Initializing the console to write to
    device_t *dev;
    gcd_t *gcd;

    //Returning 0 in case the console was not found
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


    //writing 1 character at a time to the terminal
    int i = 0;
    while (i < (int)length)
    {
        if (cc[i] == '\0')
        {
            break;
        }
        len = gcd->write(gcd, &cc[i], 1);
        written_len += len;
        i++;
    }

    DEBUG("debuginit", "\n\nEnding write_tty\n\n\n\n\n");
    DEBUG("debuginit", "Written bytes: %d\n", written_len);
    return written_len;
}