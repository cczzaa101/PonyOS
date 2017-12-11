/* credit to http://wiki.osdev.org/Interrupt_Service_Routines */
#ifndef IHW
#define IHW
#ifndef ASM
#include "x86_desc.h"
extern void systemcall_handler_wrapper();
/* assembly wrapper for keyboard interrupt handler */
extern void keyboard_handler_wrapper();
/* assembly wrapper for rtc interrupt handler */
extern void rtc_handler_wrapper();
/* assembly wrapper for general interrupt handler */
extern void general_handler_wrapper();

extern void pit_handler_wrapper();

extern void back_to_user_mode(int entry);

#endif
#endif
