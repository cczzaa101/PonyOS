#include "i8259.h"
#include "lib.h"
#include "tests.h"
/* rtc initialization */
extern void rtc_init();
/* handle rtc interrupts */
extern void rtc_interrupt_handler();
/* open function for rtc */
int32_t rtc_open();
/* write function for rtc */
int32_t rtc_write(int * freq, int size);
/* read functino for rtc */
int32_t rtc_read();
/* close function for rtc */
int32_t rtc_close();
