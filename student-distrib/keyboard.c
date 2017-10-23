#include "lib.h"
#include "i8259.h"
#include "keyboard.h"
#define KEYBOARD_DATA_PORT 0x60
#define KEYBOARD_STATUS_PORT 0x64
#define KEYBOARD_COMMAND_PORT 0x64
#define KEYBOARD_IRQ_NUM 1

void keyboard_init()
{
    /* keyboard irq locate at 1*/
    enable_irq(KEYBOARD_IRQ_NUM);
}

void keyboard_interrupt_handler()
{
    cli(); //disable interrupts
    clear();
    unsigned char keyboard_buffer;
    while( (inb(KEYBOARD_STATUS_PORT)&0x1)>0 ) //the buffer is still full
    {
        keyboard_buffer = inb(KEYBOARD_DATA_PORT);
        printf("%c",keyboard_buffer);
    }
    send_eoi(KEYBOARD_IRQ_NUM);
    sti(); //re-enable interrupts
}
