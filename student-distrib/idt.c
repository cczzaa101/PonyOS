#include "x86_desc.h"
#include "idt.h"
#include "lib.h"
#include "interrupt_handler_wrapper.h"
#define SYSCALL_INDEX 0x80
#define INTERRUPT_START_INDEX 32
#define PIC_OFFSET 0x20

/*print exception information*/
/* exception handler
input,output:None
Effect:disable interrupts, clear screen and display exception info. Enter while(1)loop in the end.*/
void exception_de()
{
    cli();
    clear();
    printf("0:Divide by zero!");
    while(1);
}

/* exception handler
input,output:None
Effect:disable interrupts, clear screen and display exception info. Enter while(1)loop in the end.*/
void exception_db()
{
    cli();
    clear();
    printf("1:Debug exception!");
    while(1);
}

/* exception handler
input,output:None
Effect:disable interrupts, clear screen and display exception info. Enter while(1)loop in the end.*/
void exception_nmi()
{
    cli();
    clear();
    printf("2:Non-maskable interrupt!");
    while(1);
}

/* exception handler
input,output:None
Effect:disable interrupts, clear screen and display exception info. Enter while(1)loop in the end.*/
void exception_bp()
{
    cli();
    clear();
    printf("3:Breakpoint Exception!");
    while(1);
}

/* exception handler
input,output:None
Effect:disable interrupts, clear screen and display exception info. Enter while(1)loop in the end.*/
void exception_of()
{
    cli();
    clear();
    printf("4:overflow!");
    while(1);
}

/* exception handler
input,output:None
Effect:disable interrupts, clear screen and display exception info. Enter while(1)loop in the end.*/
void exception_br()
{
    cli();
    clear();
    printf("5:bound range exceeded!");
    while(1);
}

/* exception handler
input,output:None
Effect:disable interrupts, clear screen and display exception info. Enter while(1)loop in the end.*/
void exception_ud()
{
    cli();
    clear();
    printf("6:Invalid Opcode!");
    while(1);
}

/* exception handler
input,output:None
Effect:disable interrupts, clear screen and display exception info. Enter while(1)loop in the end.*/
void exception_nm()
{
    cli();
    clear();
    printf("7:Device Not Available!");
    while(1);
}

/* exception handler
input,output:None
Effect:disable interrupts, clear screen and display exception info. Enter while(1)loop in the end.*/
void exception_df()
{
    cli();
    clear();
    printf("8:Double Fault!");
    while(1);
}

/* exception handler
input,output:None
Effect:disable interrupts, clear screen and display exception info. Enter while(1)loop in the end.*/
void exception_cs()
{
    cli();
    clear();
    printf("9:Coprocessor Segment Overrun!");
    while(1);
}

/* exception handler
input,output:None
Effect:disable interrupts, clear screen and display exception info. Enter while(1)loop in the end.*/
void exception_ts()
{
    cli();
    clear();
    printf("10:Invalid TSS!");
    while(1);
}

/* exception handler
input,output:None
Effect:disable interrupts, clear screen and display exception info. Enter while(1)loop in the end.*/
void exception_np()
{
    cli();
    clear();
    printf("11:Segment Not Present!");
    while(1);
}

/* exception handler
input,output:None
Effect:disable interrupts, clear screen and display exception info. Enter while(1)loop in the end.*/
void exception_ss()
{
    cli();
    clear();
    printf("12:Stack Fault!");
    while(1);
}

/* exception handler
input,output:None
Effect:disable interrupts, clear screen and display exception info. Enter while(1)loop in the end.*/
void exception_gp()
{
    cli();
    clear();
    printf("13:General Protection Exception!");
    while(1);
}

/* exception handler
input,output:None
Effect:disable interrupts, clear screen and display exception info. Enter while(1)loop in the end.*/
void exception_pf()
{
    cli();
    clear();
    printf("14:Page-Fault!");
    while(1);
}

/* exception handler
input,output:None
Effect:disable interrupts, clear screen and display exception info. Enter while(1)loop in the end.*/
void exception_rs()
{
    cli();
    clear();
    printf("15:reserved, test error!");
    while(1);
}

/* exception handler
input,output:None
Effect:disable interrupts, clear screen and display exception info. Enter while(1)loop in the end.*/
void exception_mf()
{
    cli();
    clear();
    printf("16:FPU Floating-Point Error!");
    while(1);
}

/* exception handler
input,output:None
Effect:disable interrupts, clear screen and display exception info. Enter while(1)loop in the end.*/
void exception_ac()
{
    cli();
    clear();
    printf("17:Alignment Check Exception!");
    while(1);
}

/* exception handler
input,output:None
Effect:disable interrupts, clear screen and display exception info. Enter while(1)loop in the end.*/
void exception_mc()
{
    cli();
    clear();
    printf("18:Machine-Check Exception!");
    while(1);
}

/* exception handler
input,output:None
Effect:disable interrupts, clear screen and display exception info. Enter while(1)loop in the end.*/
void exception_xf()
{
    cli();
    clear();
    printf("19:SIMD Floating-Point Exception!");
    while(1);
}

/* general handler
input,output: None
Called when: undefined interrupts happen
Effect: clear and print info, restore interrupts
*/
void general_handler()
{
    cli();
    clear();
    printf("out of the world, there is you interruption.");
    sti();
}

/*initialize IDT
input, output: none
side effects: idt table is changed
*/
void initialize_idt()
{
    lidt(idt_desc_ptr);
    /*
    typedef union idt_desc_t {
    uint32_t val[2];
    struct {
        uint16_t offset_15_00;
        uint16_t seg_selector;
        uint8_t  reserved4;
        uint32_t reserved3 : 1;
        uint32_t reserved2 : 1;
        uint32_t reserved1 : 1;
        uint32_t size      : 1;
        uint32_t reserved0 : 1;
        uint32_t dpl       : 2;
        uint32_t present   : 1;
        uint16_t offset_31_16;
    } __attribute__ ((packed));
    } idt_desc_t;
    */
    int i;
    for(i = 0; i< NUM_VEC; i++)
    {
        //offset set by set_idt
        if(i == SYSCALL_INDEX ) //system call's dpl should be 3
            idt[i].dpl = 3;
        else idt[i].dpl = 0; // exception level for default

        idt[i].present = 1; // 1 for used interrupt, 0 for unused
        idt[i].seg_selector = KERNEL_CS;
        idt[i].reserved0 = 0; //storage segment, always 0
        idt[i].size = 1; //1 for 32bit
        idt[i].reserved1 = 1;
        idt[i].reserved2 = 1;
        idt[i].reserved4 = 0;//unused bits
        if(i>=0x20)
        {
            idt[i].reserved3 = 0; //use interrupt gate for interrupts
        }
        else idt[i].reserved3 = 1; //use trap gate for exceptions
        SET_IDT_ENTRY(idt[i], general_handler_wrapper);

    }
    SET_IDT_ENTRY(idt[PIC_OFFSET + 1], keyboard_handler_wrapper);  //keyboard at irq 1
    SET_IDT_ENTRY(idt[PIC_OFFSET + 8], rtc_handler_wrapper);  //rtc at irq 8

    /* combined idt entries with exception handler */
	SET_IDT_ENTRY(idt[0], exception_de);
    SET_IDT_ENTRY(idt[1], exception_db);
    SET_IDT_ENTRY(idt[2], exception_nmi);
    SET_IDT_ENTRY(idt[3], exception_bp);
    SET_IDT_ENTRY(idt[4], exception_of);
    SET_IDT_ENTRY(idt[5], exception_br);
    SET_IDT_ENTRY(idt[6], exception_ud);
    SET_IDT_ENTRY(idt[7], exception_nm);
    SET_IDT_ENTRY(idt[8], exception_df);
    SET_IDT_ENTRY(idt[9], exception_cs);
    SET_IDT_ENTRY(idt[10], exception_ts);
    SET_IDT_ENTRY(idt[11], exception_np);
    SET_IDT_ENTRY(idt[12], exception_ss);
    SET_IDT_ENTRY(idt[13], exception_gp);
    SET_IDT_ENTRY(idt[14], exception_pf);
    SET_IDT_ENTRY(idt[15], exception_rs);
    SET_IDT_ENTRY(idt[16], exception_mf);
    SET_IDT_ENTRY(idt[17], exception_ac);
    SET_IDT_ENTRY(idt[18], exception_mc);
    SET_IDT_ENTRY(idt[19], exception_xf);

    SET_IDT_ENTRY(idt[SYSCALL_INDEX], systemcall_wrapper );
}
