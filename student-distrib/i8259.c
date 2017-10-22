/* i8259.c - Functions to interact with the 8259 interrupt controller
 * vim:ts=4 noexpandtab
 */
//credit to code from http://wiki.osdev.org/8259_PIC#Common_Definitions
#include "i8259.h"
#include "lib.h"

/* Interrupt masks to determine which interrupts are enabled and disabled */
uint8_t master_mask; /* IRQs 0-7  */
uint8_t slave_mask;  /* IRQs 8-15 */

/* Initialize the 8259 PIC */
void i8259_init(void) {
    OUTB(MASTER_8259_PORT, ICW1);
    OUTB(SLAVE_8259_PORT, ICW1);
    
    OUTB(MASTER_8259_PORT, ICW2_MASTER);
    OUTB(SLAVE_8259_PORT, ICW2_SLAVE);
    
    OUTB(MASTER_8259_PORT, ICW3_MASTER);
    OUTB(SLAVE_8259_PORT, ICW3_SLAVE);
    
    OUTB(MASTER_8259_PORT, ICW4);
    OUTB(SLAVE_8259_PORT, ICW4);
    
    OUTB(MASTER_8259_DATA, 0xfb); //enable irq2 for slave
    OUTB(SLAVE_8259_DATA, 0xff); 
}

/* Enable (unmask) the specified IRQ */
void enable_irq(uint32_t irq_num) {
    if(irq_num < 8)
    {
        INB(MASTER_8259_DATA, master_mask);
        int testDigit = (1<<irq_num);
        if( (master_mask & testDigit) !=0 )
            master_mask -= testDigit;
        OUTB(MASTER_8259_DATA, master_mask);
    }
    else
    {
        irq_num-=8;
        INB(SLAVE_8259_DATA, slave_mask);
        int testDigit = (1<<irq_num);
        if( (slave_mask & testDigit) !=0 )
            slave_mask -= testDigit;
        OUTB(SLAVE_8259_DATA, slave_mask);
    }
}

/* Disable (mask) the specified IRQ */
void disable_irq(uint32_t irq_num) {
    if(irq_num < 8)
    {
        INB(MASTER_8259_DATA, master_mask);
        int testDigit = (1<<irq_num);
        if( (master_mask & testDigit) ==0 )
            master_mask += testDigit;
        OUTB(MASTER_8259_DATA, master_mask);
    }
    else
    {
        irq_num-=8;
        INB(SLAVE_8259_DATA, slave_mask);
        int testDigit = (1<<irq_num);
        if( (slave_mask & testDigit) ==0 )
            slave_mask += testDigit;
        OUTB(SLAVE_8259_DATA, slave_mask);
    }
}

/* Send end-of-interrupt signal for the specified IRQ */
void send_eoi(uint32_t irq_num) {
    if(irq_num < 8)
    {
        OUTB(MASTER_8259_PORT, EOI);
    }
    else{
        OUTB(SLAVE_8259_PORT, EOI);
    }
    
}
