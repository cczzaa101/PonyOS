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
/* input, output: none */
void i8259_init(void) {
    outb(ICW1, MASTER_8259_PORT);
    outb(ICW1, SLAVE_8259_PORT);
    
    outb(ICW2_MASTER, MASTER_8259_DATA);
    outb(ICW2_SLAVE, SLAVE_8259_DATA);
    
    outb(ICW3_MASTER, MASTER_8259_DATA);
    outb(ICW3_SLAVE, SLAVE_8259_DATA);
    
    outb(ICW4, MASTER_8259_DATA);
    outb(ICW4, SLAVE_8259_DATA);
    
    outb(0xfb, MASTER_8259_DATA); //set mask to 0xfb(11111011)enable irq2 for slave
    outb(0xff, SLAVE_8259_DATA);  //mask slave pic
    
    master_mask = inb(MASTER_8259_DATA);
    slave_mask = inb(SLAVE_8259_DATA);
}

/* Enable (unmask) the specified IRQ; 
 * Input: uint32_t irq_num = the specified IRQ number;*/

void enable_irq(uint32_t irq_num) {
    if(irq_num < 8) //0-7 for master
    {
        master_mask = inb(MASTER_8259_DATA);
        int testDigit = (1<<irq_num);
        if( (master_mask & testDigit) !=0 )
            master_mask -= testDigit;
        outb(master_mask, MASTER_8259_DATA);
    }
    else
    {
        irq_num-=8; //8-15 for slave, subtract 8
        slave_mask = inb(SLAVE_8259_DATA);
        int testDigit = (1<<irq_num);
        if( (slave_mask & testDigit) !=0 )
            slave_mask -= testDigit;
        outb(slave_mask, SLAVE_8259_DATA);
    }
    master_mask = inb(MASTER_8259_DATA);
    slave_mask = inb(SLAVE_8259_DATA);
}

/* Disable (mask) the specified IRQ 
 * Input: uint32_t irq_num = the specified IRQ number;*/
void disable_irq(uint32_t irq_num) {
    if(irq_num < 8) //0-7 for master
    {
        master_mask = inb(MASTER_8259_DATA);
        int testDigit = (1<<irq_num);
        if( (master_mask & testDigit) ==0 )
            master_mask += testDigit;
        outb(master_mask, MASTER_8259_DATA);
    }
    else
    {
        irq_num-=8;//8-15 for slave, subtract 8
        slave_mask = inb(SLAVE_8259_DATA);
        int testDigit = (1<<irq_num);
        if( (slave_mask & testDigit) ==0 )
            slave_mask += testDigit;
        outb(slave_mask, SLAVE_8259_DATA);
    }
    master_mask = inb(MASTER_8259_DATA);
    slave_mask = inb(SLAVE_8259_DATA);
}

/* Send end-of-interrupt signal for the specified IRQ */
/* End-of-interrupt byte.  This gets OR'd with
 * the interrupt number and sent out to the PIC
 * to declare the interrupt finished 
 * Input: uint32_t irq_num = the specified IRQ number;*/
void send_eoi(uint32_t irq_num) {
    if(irq_num < 8) //0-7 for master
    {
        outb(EOI|irq_num, MASTER_8259_PORT);
    }
    else{
        irq_num-=8; //8-15 for slave, subtract 8
        outb(EOI|irq_num, SLAVE_8259_PORT);
        outb(EOI|2, MASTER_8259_PORT); //eoi at master's irq2, where slave connects
    }
    
}
