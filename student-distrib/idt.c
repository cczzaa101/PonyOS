#include "x86_desc.h"
#include "idt.h"
#include "lib.h"
#include "interrupt_handler_wrapper.h"
#define SYSCALL_INDEX 0x80
#define INTERRUPT_START_INDEX 32
#define PIC_OFFSET 0x20
void exception_de()
{
    printf("0:Divide by zero!");
}

void exception_db()
{
    printf("1:Debug exception!");
}

void exception_nmi()
{
    printf("2:Non-maskable interrupt!");
}

void exception_bp()
{
    printf("3:Breakpoint Exception!");
}

void exception_of()
{
    printf("4:overflow!");
}

void exception_br()
{
    printf("5:bound range exceeded!");
}
/////////////////////////////////////
void exception_ud()
{
    printf("6:Invalid Opcode!");
}

void exception_nm()
{
    printf("7:Device Not Available!");
}

void exception_df()
{
    printf("8:Double Fault!");
}

void exception_cs()
{
    printf("9:Coprocessor Segment Overrun!");
}

void exception_ts()
{
    printf("10:Invalid TSS!");
}

void exception_np()
{
    printf("11:Segment Not Present!");
}

void exception_ss()
{
    printf("12:Stack Fault!");
}

void exception_gp()
{
    printf("13:General Protection Exception!");
}

void exception_pf()
{
    printf("14:Page-Fault!");
}

void exception_mf()
{
    printf("16:FPU Floating-Point Error!");
}

void exception_ac()
{
    printf("17:Alignment Check Exception!");
}

void exception_mc()
{
    printf("18:Machine-Check Exception!");
}

void exception_xf()
{
    printf("19:SIMD Floating-Point Exception!");
}

void general_handler()
{
    cli();
    printf("out of the world, there is you interruption.");
    sti();
}

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
        if(i>=INTERRUPT_START_INDEX)
        {
            idt[i].reserved3 = 0; //use interrupt gate for interrupts
            SET_IDT_ENTRY(idt[i], general_handler);
        }
        else idt[i].reserved3 = 1;
        
    }
    SET_IDT_ENTRY(idt[PIC_OFFSET + 1], keyboard_handler_wrapper);  //keyboard at irq 1
    SET_IDT_ENTRY(idt[PIC_OFFSET + 8], rtc_handler_wrapper);  //rtc at irq 8
    
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
    SET_IDT_ENTRY(idt[16], exception_mf);
    SET_IDT_ENTRY(idt[17], exception_ac);
    SET_IDT_ENTRY(idt[18], exception_mc);
    SET_IDT_ENTRY(idt[19], exception_xf);
}
