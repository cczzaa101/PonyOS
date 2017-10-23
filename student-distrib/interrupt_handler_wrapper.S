/* credit to http://wiki.osdev.org/Interrupt_Service_Routines */
#define ASM     1
#include "x86_desc.h"
.global keyboard_handler_wrapper
.global rtc_handler_wrapper
.global general_handler_wrapper
.align 4

/*
assembly wrapper for keyboard handler
input: None
output: None

*/    
keyboard_handler_wrapper:
    pusha
    cld
    call keyboard_interrupt_handler
    popa
    iret

/*
assembly wrapper for rtc handler
input: None
output: None
effect: called actual handler and do iret, restore registers.
*/    
rtc_handler_wrapper:
    pusha
    cld
    call rtc_interrupt_handler
    popa
    iret

/*
assembly wrapper for rtc handler
input: None
output: None
effect: called actual handler and do iret, restore registers.
*/  
general_handler_wrapper:
    pusha
    cld
    call general_handler
    popa
    iret
