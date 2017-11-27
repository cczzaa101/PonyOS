#include "i8259.h"
#include "lib.h"
#include "rtc.h"
#include "tests.h"
#define RTC_REGISTER_PORT 0x70
#define RTC_DATA_PORT 0x71
#define NMI_MASK 0x80
#define RTC_IRQ_NUM 0x08
/* credit to http://wiki.osdev.org/RTC */

volatile int next_interrupt = 0;
/*Initialize RTC
input,output:none
Effects: enable rtc pulsing at irq 8
*/
void rtc_init()
{
    unsigned rate = 0x06; //32768>>(6-1) = 1024HZ
    next_interrupt = 0;
    cli(); //When programming the RTC, it is important that the NMI (non-maskable-interrupt) and other interrupts are disabled.

    outb(NMI_MASK + 0x0A, RTC_REGISTER_PORT); //select port A
    outb(0x20, RTC_DATA_PORT); //magic number to write to initialize

    /*set rate*/
    outb(NMI_MASK + 0x0A, RTC_REGISTER_PORT); //select port A
    unsigned char prev =  inb(RTC_DATA_PORT);
    outb(NMI_MASK + 0x0A, RTC_REGISTER_PORT); //select port A
    outb((prev&0xF0)|rate, RTC_DATA_PORT); //&0xf0 to clear lowest 4bits, | rate to reset the rate

    /*turn on irq8*/
    outb(NMI_MASK + 0x0B, RTC_REGISTER_PORT); //select port B
    prev = inb(RTC_DATA_PORT);
    outb(NMI_MASK + 0x0B, RTC_REGISTER_PORT); //select port B
    outb(prev|0x40, RTC_DATA_PORT);//use 0x40 to turn on at irq8


    /*unmask irq*/
    enable_irq(RTC_IRQ_NUM);
    sti();
}

/*RTC interrupt handler
input,output:none
side effects: calling interruption_test
*/
void rtc_interrupt_handler()
{
    cli();
    next_interrupt = 1;
    interruption_test('r',0);
    /*check register C*/
    //clear();
    //printf("rtc is running");
    outb(NMI_MASK+0x0c, RTC_REGISTER_PORT); //select C register
    char temp = inb(RTC_DATA_PORT); //meaningless steps
    temp = '\n';
    send_eoi(RTC_IRQ_NUM);

    sti();
}

/*helper function for setting frequency*/
void set_freq(int freq)
{
    /*set rate*/
    int pwr = -1,rate;
    while(freq > 0 )
    {
        freq>>=1;
        pwr++;
    }
    rate = 15 - pwr + 1; // 15 is 2^15 = 32768
    outb(NMI_MASK + 0x0A, RTC_REGISTER_PORT); //select port A
    unsigned char prev =  inb(RTC_DATA_PORT);
    outb(NMI_MASK + 0x0A, RTC_REGISTER_PORT); //select port A
    outb((prev&0xF0)|rate, RTC_DATA_PORT); //&0xf0 to clear lowest 4bits, | rate to reset the rate
}

/*RTC open function
input: none
output: 0
side effects: set RTC freq to 2HZ
*/
int32_t rtc_open()
{
    cli();
    set_freq(2);
    sti();
    return 0;
}

/*RTC close function
input: none
output: 0
side effects: None
*/
int32_t rtc_close()
{
    return 0;
}


/*RTC read function
input: none
output: 0
side effects: should block until the next interrupt, then return 0.
*/
int32_t rtc_read()
{
    sti();
    while(next_interrupt == 0 ) {};
    next_interrupt = 0;
    return 0;
}

int32_t rtc_read_wrapper(uint32_t inode, uint32_t offset, uint8_t* buf, uint32_t count)
{
    return rtc_read();
}

/*RTC write function
input: freq = frequency to set, size
output: 0
side effects: should block until the next interrupt, then return 0.
*/
int32_t rtc_write(int * freq, int size)
{
    /*check freq validity*/
    cli();
    if(freq == NULL) { sti(); return -1; }
    if(size!= sizeof(int)) { sti(); return -1; }
    int temp = *freq, flag = 0;
    if(temp<=0) { sti(); return -1; }
    if(temp>1024) temp = 1024;

    while(temp !=0)
    {
        if( ((temp&1)!=0)  && ( temp!=1 ) ) flag = 1;
        temp>>=1;
    }
    if(flag==1) { sti(); return -1; } // freq is not power of 2

    set_freq(*freq);
    sti();
    return 0;
}

//See rtc_write
int32_t rtc_write_wrapper(uint32_t inode, uint32_t offset, uint8_t* buf, uint32_t count)
{
    return rtc_write((int32_t * )buf,sizeof(int32_t));
}
