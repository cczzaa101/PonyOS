/* credit to http://wiki.osdev.org/Interrupt_Service_Routines */
#ifndef IHW
#define IHW
#ifndef ASM
#include "x86_desc.h"

extern void keyboard_handler_wrapper();
extern void rtc_handler_wrapper(); 
#endif
#endif
