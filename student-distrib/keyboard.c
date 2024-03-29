#include "lib.h"
#include "i8259.h"
#include "keyboard.h"
#include "scancode.h"
#include "tests.h"
#include "systemcall.h"
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
#define PRINT_LIM 1025*2
#define TERMINAL_NUM 3
int8_t pressed_key[TOTAL_KEY_NUM];
unsigned char keyboard_buffer[TERMINAL_NUM][TOTAL_KEY_NUM+1];
unsigned char print_buffer[TERMINAL_NUM][PRINT_LIM];
int cap_status;
int cursor_ind[TERMINAL_NUM];
int hold_num;
int terminal_read_ready[TERMINAL_NUM];
int need_nl = 0;
volatile int terminal_2_running = 0, terminal_3_running = 0;
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
    memset(keyboard_buffer, 0 , sizeof(keyboard_buffer));
    memset(cursor_ind, 0 , sizeof(cursor_ind));
    memset(terminal_read_ready, 0,  sizeof(terminal_read_ready));
    hold_num = 0;
    cap_status = 0;
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
/* processing scancode, change keyboard buffer
input: c = scancode*/
void scancode_processing(unsigned char c)
{
    //int i;
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
            memset(keyboard_buffer[get_display_terminal()], 0 , sizeof(keyboard_buffer[get_display_terminal()]) );
            cursor_ind[get_display_terminal()] = 0;
        }
        else if (  (pressed_key[SCANCODE_LEFTALT]==1)  )
        {
            if(c == SCANCODE_F1)
                set_disiplay_terminal(0);
            if(c == SCANCODE_F2)
            {
                set_disiplay_terminal(1);

                if(!terminal_2_running)
                {
                    if(get_empty_pid()==-1) return;
                    terminal_2_running = 1;
                    //sti();
                    execute_with_terminal_num((unsigned char *)"shell",1,1);
                }

            }
            if(c == SCANCODE_F3)
            {
                set_disiplay_terminal(2);

                if(!terminal_3_running)
                {
                    if(get_empty_pid()==-1) return;
                    terminal_3_running = 1;
                    //sti();
                    execute_with_terminal_num((unsigned char *)"shell",2,1);
                }

            }
            //set_active_terminal( get_active_terminal() );
        }
        else if(scancodes_map[c]!=0)
        {
            if(cursor_ind[get_display_terminal()]>=MAX_KEY_IND+1)
            {
            }
            else
            {
                keyboard_buffer[get_display_terminal()][ cursor_ind[get_display_terminal()]++ ] = character_convert(c);
                putc_scroll_display(character_convert(c));
            }
        }
    }

    if(c==SCANCODE_ENTER)
    {
        memset(print_buffer[get_display_terminal()],0, sizeof(print_buffer[get_display_terminal()]));
        memcpy(print_buffer[get_display_terminal()], keyboard_buffer[get_display_terminal()], sizeof(keyboard_buffer[get_display_terminal()]));
        //putc_scroll('\n');
        memset(keyboard_buffer[get_display_terminal()], 0 , sizeof(keyboard_buffer[get_display_terminal()]) );
        terminal_read_ready[get_display_terminal()] = 1;
        cursor_ind[get_display_terminal()] = 0;
    }

    if(c==SCANCODE_BACKSPACE)
    {
        if(cursor_ind[get_display_terminal()]!= 0)
        {
            keyboard_buffer[get_display_terminal()][ --cursor_ind[get_display_terminal()] ] = '\0';
            erase_last_ch_display();
        }
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
    send_eoi(KEYBOARD_IRQ_NUM);
    //clear();
    unsigned char placeholder;
    while( (inb(KEYBOARD_STATUS_PORT)&0x1)>0 ) //use 0x1 to check the last bit, whether the buffer is still full.
    {
        placeholder = inb(KEYBOARD_DATA_PORT);
        scancode_processing(placeholder);
        interruption_test('k', placeholder);
        //printf("%c",keyboard_buffer);
    }
    sti(); //re-enable interrupts
}
/*terminnal open function*/
int32_t terminal_open()
{
    return 0;
}
/*terminnal write function
input: buf = the buffer to write from, count = length*/
int32_t terminal_write(char* buf, int count)
{
    cli();
    int lim = sizeof(print_buffer[get_active_terminal()]);
    if(count<lim) lim = count;
    memcpy( print_buffer[get_active_terminal()], buf, lim );
    print_buffer[get_active_terminal()][lim] = 0;

    puts_scroll((char*)print_buffer[get_active_terminal()],lim);
    sti();
    //putc_scroll('\n');
    return count;
}

// See terminal_write
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
    while(terminal_read_ready[get_active_terminal()]!=1)
    {
    }
    memcpy(buf, print_buffer[get_active_terminal()], count);
    terminal_read_ready[get_active_terminal()] = 0;
    return strlen(buf);
}

//see terminal_read
int32_t terminal_read_wrapper(uint32_t inode, uint32_t offset, uint8_t* buf, uint32_t count)
{
    return terminal_read((char*)buf, (int)count);
}
