#include "i8259.h"
#include "lib.h"
#include "tests.h"
/* rtc initialization */
extern void rtc_init();
/* handle rtc interrupts */
extern void rtc_interrupt_handler();
