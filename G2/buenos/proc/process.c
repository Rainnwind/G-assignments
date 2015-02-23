/*
 * Process startup.
 *
 * Copyright (C) 2003-2005 Juha Aatrokoski, Timo Lilja,
 *       Leena Salmela, Teemu Takanen, Aleksi Virtanen.
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
 * $Id: process.c,v 1.11 2007/03/07 18:12:00 ttakanen Exp $
 *
 */

#include "proc/process.h"
#include "proc/elf.h"
#include "kernel/thread.h"
#include "kernel/assert.h"
#include "kernel/interrupt.h"
#include "kernel/config.h"
#include "fs/vfs.h"
#include "drivers/yams.h"
#include "vm/vm.h"
#include "vm/pagepool.h"
#include "kernel/sleepq.h"

//Gives a way to get lock of thread_table and entries of thread_table
extern spinlock_t thread_table_slock;
extern thread_table_t thread_table[CONFIG_MAX_THREADS];


/** @name Process startup
 *
 * This module contains facilities for managing userland processes.
 */

process_control_block_t process_table[PROCESS_MAX_PROCESSES];

/** Spinlock which must be held when manipulating the process table */
spinlock_t process_table_slock;

/**
 * Starts one userland process. The thread calling this function will
 * be used to run the process and will therefore never return from
 * this function. This function asserts that no errors occur in
 * process startup (the executable file exists and is a valid ecoff
 * file, enough memory is available, file operations succeed...).
 * Therefore this function is not suitable to allow startup of
 * arbitrary processes.
 *
 * @executable The name of the executable to be run in the userland
 * process
 */
void process_start(uint32_t process_id) {
    thread_table_t *my_entry;
    pagetable_t *pagetable;
    uint32_t phys_page;
    context_t user_context;
    uint32_t stack_bottom;
    elf_info_t elf;
    openfile_t file;

    int i;

    interrupt_status_t intr_status;

    my_entry = thread_get_current_thread_entry();
    my_entry->process_id = process_id;

    /* If the pagetable of this thread is not NULL, we are trying to
       run a userland process for a second time in the same thread.
       This is not possible. */
    KERNEL_ASSERT(my_entry->pagetable == NULL);

    pagetable = vm_create_pagetable(thread_get_current_thread());
    KERNEL_ASSERT(pagetable != NULL);

    intr_status = _interrupt_disable();
    my_entry->pagetable = pagetable;
    _interrupt_set_state(intr_status);

    file = vfs_open(process_table[process_id].executable);
    /* Make sure the file existed and was a valid ELF file */
    KERNEL_ASSERT(file >= 0);
    KERNEL_ASSERT(elf_parse_header(&elf, file));

    /* Trivial and naive sanity check for entry point: */
    KERNEL_ASSERT(elf.entry_point >= PAGE_SIZE);

    /* Calculate the number of pages needed by the whole process
       (including userland stack). Since we don't have proper tlb
       handling code, all these pages must fit into TLB. */
    KERNEL_ASSERT(elf.ro_pages + elf.rw_pages + CONFIG_USERLAND_STACK_SIZE
          <= _tlb_get_maxindex() + 1);

    /* Allocate and map stack */
    for(i = 0; i < CONFIG_USERLAND_STACK_SIZE; i++) {
        phys_page = pagepool_get_phys_page();
        KERNEL_ASSERT(phys_page != 0);
        vm_map(my_entry->pagetable, phys_page,
               (USERLAND_STACK_TOP & PAGE_SIZE_MASK) - i*PAGE_SIZE, 1);
    }

    /* Allocate and map pages for the segments. We assume that
       segments begin at page boundary. (The linker script in tests
       directory creates this kind of segments) */
    for(i = 0; i < (int)elf.ro_pages; i++) {
        phys_page = pagepool_get_phys_page();
        KERNEL_ASSERT(phys_page != 0);
        vm_map(my_entry->pagetable, phys_page,
               elf.ro_vaddr + i*PAGE_SIZE, 1);
    }

    for(i = 0; i < (int)elf.rw_pages; i++) {
        phys_page = pagepool_get_phys_page();
        KERNEL_ASSERT(phys_page != 0);
        vm_map(my_entry->pagetable, phys_page,
               elf.rw_vaddr + i*PAGE_SIZE, 1);
    }

    /* Put the mapped pages into TLB. Here we again assume that the
       pages fit into the TLB. After writing proper TLB exception
       handling this call should be skipped. */
    intr_status = _interrupt_disable();
    tlb_fill(my_entry->pagetable);
    _interrupt_set_state(intr_status);

    /* Now we may use the virtual addresses of the segments. */

    /* Zero the pages. */
    memoryset((void *)elf.ro_vaddr, 0, elf.ro_pages*PAGE_SIZE);
    memoryset((void *)elf.rw_vaddr, 0, elf.rw_pages*PAGE_SIZE);

    stack_bottom = (USERLAND_STACK_TOP & PAGE_SIZE_MASK) -
        (CONFIG_USERLAND_STACK_SIZE-1)*PAGE_SIZE;
    memoryset((void *)stack_bottom, 0, CONFIG_USERLAND_STACK_SIZE*PAGE_SIZE);

    /* Copy segments */

    if (elf.ro_size > 0) {
    /* Make sure that the segment is in proper place. */
        KERNEL_ASSERT(elf.ro_vaddr >= PAGE_SIZE);
        KERNEL_ASSERT(vfs_seek(file, elf.ro_location) == VFS_OK);
        KERNEL_ASSERT(vfs_read(file, (void *)elf.ro_vaddr, elf.ro_size)
              == (int)elf.ro_size);
    }

    if (elf.rw_size > 0) {
    /* Make sure that the segment is in proper place. */
        KERNEL_ASSERT(elf.rw_vaddr >= PAGE_SIZE);
        KERNEL_ASSERT(vfs_seek(file, elf.rw_location) == VFS_OK);
        KERNEL_ASSERT(vfs_read(file, (void *)elf.rw_vaddr, elf.rw_size)
              == (int)elf.rw_size);
    }


    /* Set the dirty bit to zero (read-only) on read-only pages. */
    for(i = 0; i < (int)elf.ro_pages; i++) {
        vm_set_dirty(my_entry->pagetable, elf.ro_vaddr + i*PAGE_SIZE, 0);
    }

    /* Insert page mappings again to TLB to take read-only bits into use */
    intr_status = _interrupt_disable();
    tlb_fill(my_entry->pagetable);
    _interrupt_set_state(intr_status);

    /* Initialize the user context. (Status register is handled by
       thread_goto_userland) */
    memoryset(&user_context, 0, sizeof(user_context));
    user_context.cpu_regs[MIPS_REGISTER_SP] = USERLAND_STACK_TOP;
    user_context.pc = elf.entry_point;

    thread_goto_userland(&user_context);

    KERNEL_PANIC("thread_goto_userland failed.");
}


void process_init() {
    spinlock_reset(&process_table_slock);
    _interrupt_disable();
    spinlock_acquire(&process_table_slock);

    for (int i = 0; i < PROCESS_MAX_PROCESSES; i++) {
        process_table[i].state = PROCESS_FREE;
        process_table[i].retval = 0;
        process_table[i].parent_id = -1;
        process_table[i].child_count = 0;
        for (int j = 0; j < PROCESS_MAX_PROCESSES; j++) {
            process_table[i].children[j] = 0;
        }
    }

    spinlock_release(&process_table_slock);
    _interrupt_enable();
}

/**
 * Entries in a process children array are either 0 or 1.
 * If an entry is 1, the number of the entry is a child process
 *
 * The function assumes that a proper table lock is already acquired
 */
void process_add_child(process_id_t parent_id, process_id_t child_id) {
    if (parent_id >= 0) {
        process_table[parent_id].child_count++;
        process_table[parent_id].children[child_id] = 1;
    }
}

/**
 * Entries in a process children array are either 0 or 1.
 * If an entry is 1, the number of the entry is a child process
 *
 * The function assumes that a proper table lock is already acquired
 */
void process_remove_child(process_id_t parent_id, process_id_t child_id) {
    if (parent_id >= 0) {
        process_table[parent_id].child_count--;
        process_table[parent_id].children[child_id] = 0;
    }
}

process_id_t process_spawn(const char *executable) {
    process_id_t parent_id = process_get_current_process();
    process_id_t process_id = PROCESS_PTABLE_FULL;
    TID_t thread_id;

    //Looking for a available process in process_table
    _interrupt_disable();
    spinlock_acquire(&process_table_slock);

    for (int i = 0; i < PROCESS_MAX_PROCESSES; i++) {
        if (process_table[i].state == PROCESS_FREE) {
            process_id = i;
            break;
        }
    }

    //process_table was full - Return error value
    if (process_id == PROCESS_PTABLE_FULL) {
        spinlock_release(&process_table_slock);
        _interrupt_enable();
        return PROCESS_PTABLE_FULL;
    }

    //process_table was not full - Assign values to the process

    stringcopy(process_table[process_id].executable, executable, EXECUTABLE_LENGTH); //Copying the executable name into process_table - Used in process_start
    process_table[process_id].state = PROCESS_RUNNING;
    process_table[process_id].retval = 0;
    process_table[process_id].parent_id = parent_id; //If -1 then this process is the root of the system (initprog)
    process_table[process_id].child_count = 0;
    for (int i = 0; i < PROCESS_MAX_PROCESSES; i++) {
        process_table[process_id].children[i] = 0;
    }

    //Appending child to it's parent in case that the parent terminates before the child
    process_add_child(parent_id, process_id);

    spinlock_release(&process_table_slock);
    _interrupt_enable();

    thread_id = thread_create(&process_start, process_id);
    thread_run(thread_id);

    _interrupt_disable();
    spinlock_acquire(&process_table_slock);
    process_table[process_id].thread_id = thread_id;
    spinlock_release(&process_table_slock);
    _interrupt_enable();
    return process_id;
}

void process_kill_children(process_id_t process_id) {
    int count = 0;
    for (int i = 0; i < PROCESS_MAX_PROCESSES; i++) {
        if (process_table[process_id].children[i] == CHILD) {
            //Romving child from current process
            process_remove_child(process_id, i);

            process_table[i].state = PROCESS_FREE;
            process_table[i].parent_id = -1;


            spinlock_acquire(&thread_table_slock);

            thread_table[process_table[i].thread_id].context->pc = (uint32_t)process_finish;

            spinlock_release(&thread_table_slock);

            if (process_table[i].child_count > 0) {
                process_kill_children(i);
            }

            process_table[process_id].child_count--;
            count++;
            if (count == process_table[process_id].child_count - 1) {
                break;
            }
        }
    }
}

/* Stop the process and the thread it runs in.  Sets the return value as
   well. */
void process_finish(int retval) {
    interrupt_status_t intr_status;
    thread_table_t *my_entry;
    my_entry = thread_get_current_thread_entry();

    intr_status = _interrupt_disable();
    spinlock_acquire(&process_table_slock);
    if (process_table[my_entry->process_id].child_count > 0) {
        process_kill_children(my_entry->process_id);
    }

    if (my_entry->process_id >= 0) {
        process_table[my_entry->process_id].state = PROCESS_ZOMBIE;
        process_table[my_entry->process_id].retval = retval;
    }

    sleepq_wake(&process_table[my_entry->process_id]);

    spinlock_release(&process_table_slock);
    _interrupt_set_state(intr_status);

    if (my_entry->pagetable != NULL) {
        vm_destroy_pagetable(my_entry->pagetable);
        my_entry->pagetable = NULL;
    }
    thread_finish();
}

int process_join(process_id_t pid) {
    interrupt_status_t intr_status;
    int retval;
    //Checking that the process_id is within range
    if (pid < 0 || pid >= PROCESS_MAX_PROCESSES) {
        return -1;
    }


    //Sleeping untill the process waiting for has finished
    intr_status = _interrupt_disable();
    spinlock_acquire(&process_table_slock);

    while (process_table[pid].state != PROCESS_ZOMBIE) {
        sleepq_add(&process_table[pid]);
        spinlock_release(&process_table_slock);
        thread_switch();
        spinlock_acquire(&process_table_slock);
    }

    //The processes waiting for has finished
    retval = process_table[pid].retval;
    process_table[pid].state = PROCESS_FREE;

    spinlock_release(&process_table_slock);
    _interrupt_set_state(intr_status);


    return retval;
}

process_id_t process_get_current_process(void) {
    return thread_get_current_thread_entry()->process_id;
}

process_control_block_t *process_get_current_process_entry(void) {
    return &process_table[process_get_current_process()];
}

process_control_block_t *process_get_process_entry(process_id_t pid) {
    return &process_table[pid];
}

/** @} */
