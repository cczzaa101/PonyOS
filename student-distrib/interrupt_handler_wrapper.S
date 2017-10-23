/* credit to http://wiki.osdev.org/Interrupt_Service_Routines */
#define ASM     1
#include "x86_desc.h"
.global keyboard_handler_wrapper
.global rtc_handler_wrapper
.align 4

    
keyboard_handler_wrapper:
    pusha
    cld
    call keyboard_interrupt_handler
    popa
    iret

rtc_handler_wrapper:
    pusha
    cld
    call rtc_interrupt_handler
    popa
    iret
