#include "lib/debug.h"
#include "drivers/gcd.h"
#include "kernel/write.h"
#include "kernel/assert.h"
#include "lib/libc.h"
#include "proc/syscall.h"


int syscall_write(uint32_t f_handle, uint32_t buffer_addr, uint32_t length)
{
    //Handling console write
    if (f_handle == FILEHANDLE_STDOUT)
    {
        DEBUG("debugreadwrite", "Starting syscall_write\n\n\n\n\n");

        //Collecting the start of the buffer - the size of it is contained by `length`
        char *cc = (char *)buffer_addr;

        int len;
        int written_len = 0;

        //Initializing the console to write to
        device_t *dev;
        gcd_t *gcd;

        //Returning -1 in case the console was not found
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

        DEBUG("debugreadwrite", "\n\nEnding syscall_write\n\n\n\n\n");
        DEBUG("debugreadwrite", "Written bytes: %d\n", written_len);
        return written_len;
    }
    //Handling file write
    else
    {
        return -1;
    }
}