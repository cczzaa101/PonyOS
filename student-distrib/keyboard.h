#include "lib.h"
#include "i8259.h"
#include "tests.h"
#include "scancode.h"
/*keyboard initialization*/
extern void keyboard_init();
/*handle keyboard interrupt*/
extern void keyboard_interrupt_handler();

int32_t terminal_read(char* buf, int count);
int32_t terminal_read_wrapper(uint32_t inode, uint32_t offset, uint8_t* buf, uint32_t count);
unsigned char character_convert(unsigned char c);
int32_t terminal_write(char* buf, int count);
int32_t terminal_write_wrapper(uint32_t inode, uint32_t offset, uint8_t* buf, uint32_t count);
int32_t terminal_close();
