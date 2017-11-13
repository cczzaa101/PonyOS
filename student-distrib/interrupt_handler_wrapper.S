/* credit to http://wiki.osdev.org/Interrupt_Service_Routines */
#define ASM     1
#include "x86_desc.h"
.global keyboard_handler_wrapper
.global rtc_handler_wrapper
.global general_handler_wrapper
.glbal back_to_user_mode
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

/*
back to usermode
https://web.archive.org/web/20160326062442/http://jamesmolloy.co.uk/tutorial_html/10.-User%20Mode.html
*/
back_to_user_mode:
    cli
    movw $USER_DS %ax
    movw %ax, %ds
    movw %ax, %es
    movw %ax, %fs
    movw %ax, %gs

    movl %esp, %eax
    pushl $USER_DS
    pushl $0x08448000
    pushf
    /* reset IF flag */
    popl %eax
    orl $0x200, %eax
    pushl $USER_CS
    movl 20(%esp), %eax
    pushl %eax
    iret

/*
systemcall wrapper
https://web.archive.org/web/20160326062442/http://jamesmolloy.co.uk/tutorial_html/10.-User%20Mode.html
*/
systemcall_wrapper:
    pusha
    /* push the arguments */
    pushl %edx
    pushl %ecx
    pushl %ebx

    call *system_call_jumptable(,%eax,4)

    popl %ebx
    popl %ecx
    popl %edx
    popa
    iret

system_call_jumptable:
    .long $0
    .long halt, execute, read, write, open, close
