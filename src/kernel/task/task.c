#include <nuttle/kernel.h>
#include <nuttle/task/task.h>
#include <nuttle/status.h>
#include <nuttle/config.h>
#include <nuttle/error.h>
#include <kerndef.h>
#include <kernmem.h>

// To keep track of running, created and removed tasks.

static NuttleTask* current = nullptr;

static NuttleTask* head = nullptr;
static NuttleTask* tail = nullptr;

static int task_init(NuttleTask* task, NuttleProcess* process);

NuttleTask* task_get_current() {
    return current;
}

NuttleTask* task_new(NuttleProcess* process) {
    int res = NUTTLE_ALL_OK;

    NuttleTask* task = mallock(sizeof(NuttleTask));

    if(ISERRP(task)) {
        res = -ENOMEM;

        goto out;
    }

    if(ISERR(res = task_init(task, process))) 
        goto out;
    
    if(head == nullptr) {
        head = task;
        tail = task;

        goto out;
    }

    task -> prev = tail;
    tail -> next = task;

    tail = task;

out: 
    if(ISERR(res) && task != nullptr) 
        task_free(task);
    
    return ISERR(res) ? ERROR_P(res) : task;
}

void task_switch(NuttleTask* task) {
    current = task;
    paging_switch(task -> chunk);
}

void task_page() {
    task_restore_user_segment_registers();

    paging_switch(task_get_current() -> chunk);
}

void task_store_frame(TaskRegisters* regs) {
    // If there is no current task, panic the kernel.

    if(task_get_current() == nullptr) 
        kernel_panic("task_store_frame(): No current task!\n");

    // The interrupt frame structure and the task register structure are 
    // built in same order. So, they can be easily casted and copied from one
    // another.

    // We are copying total 32 bytes of data (8 registers in total).

    memcpyk(&task_get_current() -> registers, regs, 8u * sizeof(uint32_t));
}

// Run the current task.

int task_run() {
    int res = NUTTLE_ALL_OK;

    if(current == nullptr) {
        res = -ENOTASK;

        goto out;
    }

    task_jump_usermode(&current -> registers);
    
out: 
    return res;
}

static void task_list_remove(NuttleTask* task) {
    if(task -> prev) 
        task -> prev -> next = task -> next;
    
    if(task -> next) 
        task -> next -> prev = task -> prev;

    if(head == task) 
        head = task -> next;
    
    if(tail == task) 
        tail = task -> prev;
    
    // Cycle through the tasks.

    if(current == task) 
        current = current -> next ? current -> next : head;
}

void task_free(NuttleTask* task) {
    // Free the paging chunk.

    if(task == nullptr) return;

    paging_free_4gb_chunk(task -> chunk);

    // Remove it from the list.

    task_list_remove(task);

    // Now free the task.

    freek(task);
}

static int task_init(NuttleTask* task, NuttleProcess* process) {
    int res = NUTTLE_ALL_OK;

    memsetk(task, 0x00, sizeof(NuttleTask));

    // Create the paging chunk.

    task -> chunk = paging_get_new_4gb_chunk(PAGING_IS_PRESENT | PAGING_IS_WRITABLE | PAGING_ACCESS_FROM_ALL);

    if(ISERRP(task -> chunk)) {
        res = -ENOMEM;

        goto out;
    }

    // Set the virtual memory locations for the user program instruction pointer, stack pointer.

    task -> registers.eip  = NUTTLE_USER_PROGRAM_VIRTUAL_ADDR;
    task -> registers.esp = NUTTLE_USER_STACK_VIRTUAL_ADDR_START;

    // Also set the selector registers.

    task -> registers.cs = NUTTLE_USER_CODE_SEGMENT_SELECTOR | 3;    // Also or'ing the RPL.
    task -> registers.ds = NUTTLE_USER_DATA_SEGMENT_SELECTOR | 3;    // Same.
    task -> registers.ss = task -> registers.ds;
    task -> registers.es = task -> registers.ds;
    task -> registers.fs = task -> registers.ds;
    task -> registers.gs = task -> registers.ds;

    // Now just simply set the process.

    task -> process = process;

out: 
    return res;
}