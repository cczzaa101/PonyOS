#include "lib.h"
#include "i8259.h"
#include "keyboard.h"
#include "scancode.h"
#include "tests.h"
#define KEYBOARD_DATA_PORT 0x60
#define KEYBOARD_STATUS_PORT 0x64
#define KEYBOARD_COMMAND_PORT 0x64
#define KEYBOARD_IRQ_NUM 1

#define LSHIFT_IND  0
#define RSHIFT_IND  1
#define CAPSLOCK_IND 2
#define CTRL_IND 3
#define RELEASE(i) (i-128)
#define MAX_KEY_IND 127
#define TOTAL_KEY_NUM 128
#define PRINT_LIM 1025
int8_t pressed_key[TOTAL_KEY_NUM];
unsigned char keyboard_buffer[TOTAL_KEY_NUM+1] = "391OS> ";
unsigned char print_buffer[PRINT_LIM];
int cap_status;
int cursor_ind;
int hold_num;
int terminal_read_ready;
int need_nl = 0;
/* keyboard initilization
 *
 * initialize the keyboard and let it begin generating interrupts
 * Inputs: None
 * Outputs: None
 * Side Effects: irq 1 is enabled
 */
void keyboard_init()
{
    /* keyboard irq locate at 1*/
    init_scancodes_map();
    enable_irq(KEYBOARD_IRQ_NUM);
    memset(pressed_key, 0 , sizeof(pressed_key) );
    hold_num = 0;
    cursor_ind = strlen("391OS> ");
    terminal_read_ready = 0;
    cap_status = 0;
    puts_scroll_refresh((char*)keyboard_buffer);
}
/* change by shift */
unsigned char character_convert(unsigned char c)
{
    if( scancodes_map[c] ==0 ) return 0;
    unsigned char res = scancodes_map[c];
    if( (cap_status == 1) && (res>='a') && (res<='z') )
        res = special_convert_map[res];
    if( (pressed_key[SCANCODE_LEFTSHIFT]==1) || (pressed_key[SCANCODE_RIGHTSHIFT]==1) )
        res = special_convert_map[res];
    return res;
}
/* processing scancode, change keyboard buffer*/
void scancode_processing(unsigned char c)
{
    int i;
    if( c>MAX_KEY_IND )
    {
        pressed_key[ RELEASE(c) ] = 0;
        hold_num--;
    }
    else
    {
        pressed_key[ c ] = 1;
        hold_num++;
        if( c== SCANCODE_CAPSLOCK ) cap_status = 1-cap_status;
        if( (c==SCANCODE_L) && (pressed_key[SCANCODE_LEFTCONTROL]==1)  )
        {
            clear();
            memset(keyboard_buffer, 0 , sizeof(keyboard_buffer) );
            memcpy(keyboard_buffer, "391OS> ", strlen("391OS> "));
            cursor_ind = strlen((char*)keyboard_buffer);
        }
        else if(scancodes_map[c]!=0)
        {
            if(cursor_ind==MAX_KEY_IND+1)
            {
                for(i=0; i<MAX_KEY_IND; i++)
                {
                    keyboard_buffer[i] = keyboard_buffer[i+1];
                }
                keyboard_buffer[MAX_KEY_IND] = character_convert(c);
                memcpy(keyboard_buffer, "391OS> ", strlen("391OS> "));
            }
            else
            {
                keyboard_buffer[cursor_ind++] = character_convert(c);
            }
        }
    }

    if(c==SCANCODE_ENTER)
    {
        memset(print_buffer,0, sizeof(print_buffer));
        int shell_offset = strlen((char*)"391OS> ");
        for(i=shell_offset; i< strlen( (char*) keyboard_buffer); i++)
            print_buffer[i - shell_offset] = keyboard_buffer[i];
        //memcpy(print_buffer, keyboard_buffer, sizeof(keyboard_buffer) );
        puts_scroll_refresh((char*)keyboard_buffer);
        //puts_scroll("\n");
        need_nl = 1;
        memset(keyboard_buffer, 0 , sizeof(keyboard_buffer) );
        memcpy(keyboard_buffer, "391OS> ", strlen((char*)"391OS> "));
        terminal_read_ready = 1;
        cursor_ind = strlen((char*)keyboard_buffer);
    }

    if(c==SCANCODE_BACKSPACE)
    {
        if(cursor_ind!= strlen((char*)"391OS> "))
            keyboard_buffer[--cursor_ind] = '\0';
        //cursor_ind--;
    }
    /*
    if( c==SCANCODE_LEFTCONTROL ) special_condition[ CTRL_IND ] = 1;
    if( c==RELEASE(SCANCODE_LEFTCONTROL) ) special_condition[ CTRL_IND ] = 0;
    if( c==SCANCODE_LEFTSHIFT ) special_condition[ LSHIFT_IND ] = 1;
    if( c==RELEASE(SCANCODE_LEFTSHIFT) ) special_condition[ LSHIFT_IND ] = 0;
    if( c==SCANCODE_RIGHTSHIFT ) special_condition[ RSHIFT_IND ] = 1;
    if( c==RELEASE(SCANCODE_RIGHTSHIFT) )special_condition[ RSHIFT_IND ] = 0;
    if( c==CAPSLOCK_IND ) special_condition[ CAPSLOCK_IND ] = ( 1 - special_condition[ CAPSLOCK_IND ]);
    */
}
/* keyboard interrupt handler
 *
 * handle the interrupt generated by keyboard
 * Inputs: None
 * Outputs: None
 * Side Effects: read ch from buffer, called the interruption test
 */
void keyboard_interrupt_handler()
{
    cli(); //disable interrupts
    //clear();
    unsigned char placeholder;
    while( (inb(KEYBOARD_STATUS_PORT)&0x1)>0 ) //use 0x1 to check the last bit, whether the buffer is still full.
    {
        placeholder = inb(KEYBOARD_DATA_PORT);
        scancode_processing(placeholder);
        interruption_test('k', placeholder);
        //printf("%c",keyboard_buffer);
    }
    if(need_nl==1) { puts_scroll("\n"); need_nl = 0; }
    puts_scroll_refresh((char*)keyboard_buffer);
    send_eoi(KEYBOARD_IRQ_NUM);
    sti(); //re-enable interrupts
}
/*terminnal open function*/
int32_t terminal_open()
{
    return 0;
}
/*terminnal write function*/
int32_t terminal_write(char* buf, int count)
{
    if(strncmp("391OS> ", buf, strlen( (char*) "391OS> ")) ==0 ) return count;
    int lim = sizeof(print_buffer);
    if(count<lim) lim = count;
    memcpy( print_buffer, buf, lim );
    print_buffer[lim] = 0;
    if(print_buffer[ strlen( (char*) print_buffer) -1] !='\n')
    {
        print_buffer[ strlen( (char*) print_buffer) +1 ] = '\0';
        print_buffer[ strlen( (char*) print_buffer) ] = '\n';
    }
    puts_scroll_refresh("");
    puts_scroll((char*)print_buffer);
    //putc_scroll('\n');
    return count;
}

int32_t terminal_write_wrapper(uint32_t inode, uint32_t offset, uint8_t* buf, uint32_t count)
{
    return terminal_write((char*)buf, (int)count);
}

/*terminnal close function*/
int32_t terminal_close()
{
    return 0;
}
/*terminnal read function*/
int32_t terminal_read(char* buf, int count)
{
    sti();
    while(terminal_read_ready!=1)
    {
    }
    memcpy(buf, print_buffer, count);
    terminal_read_ready = 0;
    return strlen(buf);
}

int32_t terminal_read_wrapper(uint32_t inode, uint32_t offset, uint8_t* buf, uint32_t count)
{
    return terminal_read((char*)buf, (int)count);
}
