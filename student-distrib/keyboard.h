#include "lib.h"
#include "i8259.h"
#include "tests.h"
#include "scancode.h"
/*keyboard initialization*/
extern void keyboard_init();
/*handle keyboard interrupt*/
extern void keyboard_interrupt_handler();
