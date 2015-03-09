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
#include "vm/tlb.h"
#include "kernel/thread.h"
#include "proc/process.h"


/* Check whether given (virtual) address is even or odd mapping
   in a pair of mappings for TLB. */
#define ADDR_IS_ON_EVEN_PAGE(addr) (!((addr) & 0x00001000))
#define ADDR_IS_ON_ODD_PAGE(addr)  ((addr) & 0x00001000)


void tlb_modified_exception(void)
{
    tlb_exception_state_t err_state;
   _tlb_get_exception_state(&err_state);
    pagetable_t *pagetable;

    pagetable = thread_get_current_thread_entry()->pagetable;
    if (pagetable == NULL) {
        KERNEL_PANIC("No pagetable in this thread - Kernel error!");
    }

    //Kernel threads have no pagetable, in case KERNEL_PANIC is not called we assume it's a user process and terminate the process
    process_finish(-1);
}

void tlb_load_exception(void)
{
    //It's supposed to do the exact same thing as tlb_store_exception
    tlb_store_exception();
}

void tlb_store_exception(void)
{
    tlb_exception_state_t err_state;
   _tlb_get_exception_state(&err_state);

    pagetable_t *pagetable;

    pagetable = thread_get_current_thread_entry()->pagetable;
    if (pagetable == NULL) {
        KERNEL_PANIC("No pagetable in this thread!");
    }


    //Fetches an index from `global` tlb
    for (int i = 0; i < PAGETABLE_ENTRIES; i++) {
        if (pagetable->entries[i].ASID == err_state.asid && pagetable->entries[i].VPN2 == err_state.badvpn2) {
            //Testing if page is even or odd
            //Then checking if page is valid
            if (ADDR_IS_ON_EVEN_PAGE(err_state.badvaddr)) {
                if (pagetable->entries[i].V0 == 0) {
                    process_finish(-1);
                    return;
                }
            } else if (ADDR_IS_ON_ODD_PAGE(err_state.badvaddr)) {
                if (pagetable->entries[i].V1 == 0) {
                    process_finish(-1);
                    return;
                }
            } else {
                //Should not be possible
                KERNEL_PANIC("ADDRESS NOT RECOGNIZED!");
            }
            _tlb_write_random(&pagetable->entries[i]);
            return;
        }
    }

    //In case the TLB was not found in the thread - We handle this as an access violation an terminate the process
    process_finish(-1);
}