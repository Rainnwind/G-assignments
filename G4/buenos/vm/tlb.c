/*
 * TLB handling
 *
 * Copyright (C) 2003 Juha Aatrokoski, Timo Lilja,
 *   Leena Salmela, Teemu Takanen, Aleksi Virtanen.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 *
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above
 *    copyright notice, this list of conditions and the following
 *    disclaimer in the documentation and/or other materials provided
 *    with the distribution.
 * 3. The name of the author may not be used to endorse or promote
 *    products derived from this software without specific prior
 *    written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE AUTHOR ``AS IS'' AND ANY EXPRESS
 * OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
 * WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED. IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR ANY
 * DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE
 * GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
 * INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY,
 * WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING
 * NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
 * SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 *
 * $Id: tlb.c,v 1.6 2004/04/16 10:54:29 ttakanen Exp $
 *
 */

#include "kernel/panic.h"
#include "kernel/assert.h"
#include "vm/tlb.h"
#include "vm/pagetable.h"
#include "kernel/spinlock.h"
#include "kernel/config.h"
#include "kernel/thread.h"
#include "vm/vm.h"

extern spinlock_t thread_table_slock;
extern thread_table_t thread_table[CONFIG_MAX_THREADS];

void tlb_modified_exception(void) {
    KERNEL_PANIC("Unhandled TLB modified exception 1");
}

void tlb_load_exception(void) {
    KERNEL_PANIC("Unhandled TLB load exception 2");
}

void tlb_store_exception(void) {
    tlb_exception_state_t exception_state;

    //Getting exception state
    _tlb_get_exception_state(&exception_state);

    // Debugging
    kprintf("TLB exception. Details:\n"
            "Failed Virtual Address: 0x%8.8x\n"
            "Virtual Page Number:    0x%8.8x\n"
            "ASID (Thread number):   %d\n",
            exception_state.badvaddr, exception_state.badvpn2, exception_state.asid);



    kprintf("ASID from thread: %d\n", thread_get_current_thread_entry()->pagetable->ASID);
    kprintf("valid_count from thread: %d\n", thread_get_current_thread_entry()->pagetable->valid_count);
    kprintf("virtual address: %d\n", exception_state.badvaddr);
    kprintf("Virtual page number: %d\n", exception_state.badvpn2);
    kprintf("ADDR_KERNEL_TO_PHYS(): %d\n", ADDR_KERNEL_TO_PHYS(exception_state.badvaddr));
    kprintf("ADDR_PHYS_TO_KERNEL(): %d\n", ADDR_PHYS_TO_KERNEL(exception_state.badvaddr));

    if (thread_get_current_thread_entry()->pagetable == NULL) {
        kprintf("It's a KERNEL thread only\n");
    } else {
        kprintf("It's a userland thread\n");
    }
    int index = _tlb_probe(thread_get_current_thread_entry()->pagetable->entries);
    if (index < 0) {
        kprintf("thread_id: %d, index: %d\n", thread_get_current_thread(), index);
        _tlb_write_random(thread_get_current_thread_entry()->pagetable->entries);
//        KERNEL_PANIC("index less than zero OMG!\n");
    }
}

/**
 * Fill TLB with given pagetable. This function is used to set memory
 * mappings in CP0's TLB before we have a proper TLB handling system.
 * This approach limits the maximum mapping size to 128kB.
 *
 * @param pagetable Mappings to write to TLB.
 *
 */

void tlb_fill(pagetable_t *pagetable)
{
    if(pagetable == NULL)
	return;

    /* Check that the pagetable can fit into TLB. This is needed until
     we have proper VM system, because the whole pagetable must fit
     into TLB. */
    KERNEL_ASSERT(pagetable->valid_count <= (_tlb_get_maxindex()+1));

    _tlb_write(pagetable->entries, 0, pagetable->valid_count);

    /* Set ASID field in Co-Processor 0 to match thread ID so that
       only entries with the ASID of the current thread will match in
       the TLB hardware. */
    _tlb_set_asid(pagetable->ASID);
}
