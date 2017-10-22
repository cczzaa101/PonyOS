#include "x86_desc.h"
#define SYSCALL_INDEX 0x80
#define INTERRUPT_START_INDEX 32
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
            ldt[i].dpl = 3
        else idt[i].dpl = 0; // exception level for default
        
        idt[i].present = 1; // 1 for used interrupt, 0 for unused
        idt[i].seg_selector = KERNEL_CS;
        idt[i].reserve0 = 0; //storage segment, always 0
        idt[i].size = 1; //1 for 32bit
        idt[i].reserve1 = 1;
        idt[i].reserve2 = 1;
        if(i>=INTERRUPT_START_INDEX) idt[i].reserve3 = 0; //use interrupt gate for interrupts
        else idt[i].reserve3 = 1;
        
    }
}