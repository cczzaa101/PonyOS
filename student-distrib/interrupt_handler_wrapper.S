/* credit to http://wiki.osdev.org/Interrupt_Service_Routines */
#define ASM     1
#include "x86_desc.h"
.global keyboard_handler_wrapper
.global rtc_handler_wrapper
.global general_handler_wrapper
.global back_to_user_mode
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
general_handler_wrapperr:
    pusha
    cld
    call general_handler
    popa
    iret

/*
systemcall wrapper
https://web.archive.org/web/20160326062442/http://jamesmolloy.co.uk/tutorial_html/10.-User%20Mode.html
*/
general_handler_wrapper:
    pushl %ebp
    pushl %esi
    pushl %edi

    /* push the arguments */
    pushl %edx
    pushl %ecx
    pushl %ebx

    /* validity check, should not greater than 7 */
    cmpl $8, %eax
    jg invalid_eax

    call *system_call_jumptable(,%eax,4)
    jmp valid_eax

invalid_eax:
    movl $1, %eax
    call *system_call_jumptable(,%eax,4)

valid_eax:
    popl %ebx
    popl %ecx
    popl %edx

    popl %edi
    popl %esi
    popl %ebp

    iret

/*
back to usermode
https://web.archive.org/web/20160326062442/http://jamesmolloy.co.uk/tutorial_html/10.-User%20Mode.html
*/
back_to_user_mode:
    cli
    movw $USER_DS,%ax
    movw %ax, %ds
    movw %ax, %es
    movw %ax, %fs
    movw %ax, %gs

    movl %esp, %eax
    pushl $USER_DS
    pushl $0x083FFFF7
    pushf
    /* reset IF flag */
    popl %eax
    orl $0x200, %eax
    pushl %eax
    pushl $USER_CS
    movl 20(%esp), %eax
    pushl %eax
    iret


system_call_jumptable:
    .long do_nothing
    .long halt, execute, read, write, open, close, getargs,vidmap
