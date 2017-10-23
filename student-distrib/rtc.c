#include "i8259.h"
#include "lib.h"
#include "rtc.h"
#define RTC_REGISTER_PORT 0x70
#define RTC_DATA_PORT 0x71
#define NMI_MASK 0x80
#define RTC_IRQ_NUM 0x08
/* credit to http://wiki.osdev.org/RTC */
void rtc_init()
{
    unsigned rate = 0x06; //32768>>5 = 1024
    cli(); //When programming the RTC, it is important that the NMI (non-maskable-interrupt) and other interrupts are disabled.
    
    outb(NMI_MASK + 0x0A, RTC_REGISTER_PORT); //select port A
    outb(0x20, RTC_DATA_PORT); //magic number to write to initialize
    
    /*set rate*/
    outb(NMI_MASK + 0x0A, RTC_REGISTER_PORT); //select port A
    unsigned char prev =  inb(RTC_DATA_PORT);
    outb(NMI_MASK + 0x0A, RTC_REGISTER_PORT); //select port A
    outb(RTC_DATA_PORT, (prev&0xF0)|rate); //&0xf0 to clear lowest 4bits, | rate to reset the rate
    
    /*turn on irq8*/
    outb(NMI_MASK + 0x0B, RTC_REGISTER_PORT); //select port B
    prev = inb(RTC_DATA_PORT);
    outb(NMI_MASK + 0x0B, RTC_REGISTER_PORT); //select port B
    outb(prev|0x40, RTC_DATA_PORT);
    
    
    /*unmask irq*/
    enable_irq(8);
    sti();
}

void rtc_interrupt_handler()
{
    cli();
    /*check register C*/
    
    outb(NMI_MASK+0x0c, RTC_REGISTER_PORT);
    char temp = inb(RTC_DATA_PORT);
    send_eoi(RTC_IRQ_NUM);
    
    sti();
}