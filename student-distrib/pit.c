#include "pit.h"
#include "lib.h"
#include "i8259.h"
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

void pit_handler()
{
    cli();
    //putc_scroll('t');
    send_eoi(PIT_IRQ_NUM);
    sti();
}
