# credit to http://wiki.osdev.org/Interrupt_Service_Routines
#include "x86_desc.h"
.globl keyboard_handler_wrapper
.globl rtc_handler_wrapper
.align 4

keyboard_handler_wrapper:
    pushad
    cld
    call keyboard_interrupt_handler
    popad
    iret

rtc_handler_wrapper:
    pushad
    cld
    call rtc_interrupt_handler
    popad
    iret
