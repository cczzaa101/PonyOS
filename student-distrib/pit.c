#include "pit.h"
#include "lib.h"
#include "i8259.h"
#include "systemcall.h"
#include "paging.h"
#define lowbit_shift 8
#define lowbit_mask 0xFF
#define PIT_BASE_FREQ 1193180
#define DESIRED_FREQ 30
#define PIT_COMMAND_BYTE 0x36
#define PIT_COMMAND_PORT 0x43
#define PIT_0_PORT 0x40
#define PIT_IRQ_NUM 0x0
/*
    credit to http://www.osdever.net/bkerndev/Docs/pit.html
*/
void pit_init()
{
    cli();
    int divisor = PIT_BASE_FREQ/DESIRED_FREQ;       /* Calculate our divisor */
    outb(PIT_COMMAND_BYTE, PIT_COMMAND_PORT);             /* Set our command byte 0x36 */
    outb(divisor & lowbit_mask, PIT_0_PORT);   /* Set low byte of divisor */
    outb(divisor >> lowbit_shift, PIT_0_PORT);     /* Set high byte of divisor */
    enable_irq(PIT_IRQ_NUM);
    sti();
}

int32_t next_pid(int pid)
{
    return (pid+1)%MAX_PID;
}

void pit_handler()
{
    cli();
    send_eoi(PIT_IRQ_NUM);
    if(get_running_process_num()==0) return;
    //putc_scroll('t');
    pcb_t* pcb;
    int i = next_pid( get_current_pcb()->pid );
    for(; i!=get_current_pcb()->pid; i = next_pid(i) )
    {
        if(!is_running(i)) continue;
        pcb = (pcb_t*) get_kernel_stack_bottom(i);
        if( pcb->child == NULL ) break;
    }

    if( i!=get_current_pcb()->pid )
    {
        /* save current esp,ebp */
        //int esp, ebp;
        asm volatile("movl %%esp, %0" : "=r" (get_current_pcb()->current_esp) );
        asm volatile("movl %%ebp, %0" : "=r" (get_current_pcb()->current_ebp) );
        setup_task_page( i );
        tss.esp0 = get_kernel_stack_bottom(i) - ASSIGNED_PCB_SIZE + MEM_DEFENSE_SIZE;
        set_current_pid(i);

        asm ("movl %0, %%esp" :: "r" (pcb->current_esp) );
        asm ("movl %0, %%ebp" :: "r" (pcb->current_ebp) );

        asm ("leave"  );
        asm ("ret" );
    }
}
