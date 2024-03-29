#include "lib.h"
#include "systemcall.h"
#include "rtc.h"
#include "keyboard.h"
#include "x86_desc.h"
#include "interrupt_handler_wrapper.h"
#include "paging.h"
char arg_buf[MAX_PID][MAX_ARG_SIZE];
char fname[MAX_ARG_SIZE];
char process_status[] = "\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0";
static int arg_available[MAX_PID] = {0};
static int current_pid=-1;
static int total_running_process = 0;
//static int kernel_stack_bottom = ((0x0800000)-(0x2000));
static int entry_point;

int get_running_process_num()
{
    return total_running_process;
}

void set_current_pid(int pid)
{
    current_pid = pid;
}

int is_running(int pid)
{
    if(pid==0) return 0;
    return process_status[pid];
}

int get_kernel_stack_bottom(int pid)
{
    return 0x800000 - ASSIGNED_PCB_SIZE*(pid+1);
}

pcb_t* get_current_pcb()
{
    /*
    int esp;
    asm ("movl %%esp, %0" : "=r" (esp) );
    esp = esp/ASSIGNED_PCB_SIZE*ASSIGNED_PCB_SIZE;
    */
    if(current_pid==-1)
        return NULL;
    else return (pcb_t*) get_kernel_stack_bottom(current_pid);
}

/*initialize pcb
  input: pid = process id
  output; none	*/
void pcb_init(int pid)
{
    int i;
    pcb_t* pcb = (pcb_t*) get_kernel_stack_bottom(pid);
    pcb->pid = pid;
    /* stdin */
    (pcb->file_array)[0].read = terminal_read_wrapper;
    (pcb->file_array)[0].write = NULL;
    (pcb->file_array)[0].flags = 1;
    /* stdout */
    (pcb->file_array)[1].read = NULL;
    (pcb->file_array)[1].write = terminal_write_wrapper;
    (pcb->file_array)[1].flags = 1;

    for(i=2; i<FDT_SIZE; i++)
    {
        (pcb->file_array)[i].flags = 0;
    }

}

/*get empty process id
input: none
output: id if found, -1 if not found*/
int32_t get_empty_pid()
{
    int i;
    for(i=1; i<MAX_PID; i++)
    {
        if( process_status[i] == 0 )
            return i;
    }
    return -1;
}

/*halt
input: status
output: 0 if successful*/
int32_t halt(int32_t status)
{
    pcb_t* pcb = get_current_pcb();
    if(total_running_process==1)
    {
        char tempBuf[EXEC_INFO_BYTES*2];
        filesys_read_by_name((unsigned char*)"shell", (unsigned char*)tempBuf, EXEC_INFO_BYTES);

        /*make a note of entry point */
        int32_t * tempBuf_int = (int32_t *) tempBuf;
        entry_point = tempBuf_int[6]; //at bytes 23~27, i.e. 28/4 = 7 for int
        back_to_user_mode(entry_point);
    }

    total_running_process--;
    process_status[ pcb->pid ] = 0;
    if(pcb->is_terminal)
    {
        sti();
        clear();
        while(1){}
    }

    setup_task_page( ((pcb_t*)(pcb->parent)) -> pid ); //temporary solution, need change later
    current_pid = ((pcb_t*)(pcb->parent)) -> pid ;
    ((pcb_t*)(pcb->parent)) -> child = NULL ;
    tss.esp0 = (int)(pcb->parent) + ASSIGNED_PCB_SIZE - MEM_DEFENSE_SIZE;
    tss.ss0 = KERNEL_DS;
    asm ("movl %0, %%esp" :: "r" (pcb->parent_esp) );
    asm ("movl %0, %%ebp" :: "r" (pcb->parent_ebp) );
    asm ("movl %0, %%eax" :: "r" (0) );

    asm ("leave"  );
    asm ("ret" );
    return 0;
}

int32_t execute(const uint8_t* command)
{
    pcb_t* pcb = get_current_pcb();
    return execute_with_terminal_num(command, pcb->terminal, 0 );
}
/*execute with filename and argument
 input: command
 output: 0 if succeful, -1 if fail*/
int32_t execute_with_terminal_num (const uint8_t* command, int terminal_num, int is_terminal)
{
    if(command == NULL) return -1;
    cli();
    int i,j,cnt = 0;

    for(i=0; i<strlen( (char*)command); i++)
        if( command[i] == ' ' ) break;

    memcpy(fname, command, i);
    fname[ i ] = '\0';

    dentry_t f_entry;
    if( read_dentry_by_name((unsigned char*)fname, &f_entry) == -1 ) return -1; //doesn't exist, return -1

    /* check if executable */
    char executable_checking[4] = { 0x7f, 0x45, 0x4c, 0x46 }; //4 magic num for executable
    char tempBuf[EXEC_INFO_BYTES*2];
    filesys_read_by_name((unsigned char*)fname, (unsigned char*)tempBuf, EXEC_INFO_BYTES);
    for(j=0; j<4; j++)
    {
        if(tempBuf[j]!= executable_checking[j])
            return -1;
    }

    /* assign PCB */
    int pid = get_empty_pid();
    if(pid==-1) return -1;
    process_status[pid] = 1;
    total_running_process++;
    pcb_init(pid);

    /* copy argument */

    for(;i<strlen( (char*)command); i++ )
        if(command[i]!=' ') break;
    for(;i<strlen( (char*)command); i++ )
        arg_buf[pid][cnt++] = command[i];
    arg_buf[pid][cnt] = '\0';
    if(strlen(arg_buf[pid])>0)
        arg_available[pid] = 1;
    else
        arg_available[pid] = 0;

    /* assign page */
    setup_task_page(pid);
    set_active_terminal(terminal_num);
    /* load user_level program */
    if(load_executable( fname, (char*)USER_PROGRAM_ADDR )==-1) return -1;

    /*make a note of entry point */
    int32_t * tempBuf_int = (int32_t *) tempBuf;
    entry_point = tempBuf_int[6]; //at bytes 23~27, i.e. 28/4 = 7 for int


    /* refresh tss */


    pcb_t* pcb = (pcb_t*) get_kernel_stack_bottom(pid);
    pcb_t* prev_pcb = get_current_pcb();

    pcb->terminal = terminal_num;
    pcb->parent = (int32_t*) prev_pcb;
    pcb->is_terminal = is_terminal;
    if(is_terminal==0)
        ((pcb_t*)(pcb->parent)) ->child = (int32_t*) pcb;

    tss.esp0 = get_kernel_stack_bottom(pid) + ASSIGNED_PCB_SIZE - MEM_DEFENSE_SIZE;
    tss.ss0 = KERNEL_DS;

    pcb->current_ebp = pcb->current_esp = tss.esp0;
    current_pid = pid;

    if(prev_pcb!=NULL)
    {
        asm volatile("movl %%esp, %0" : "=r" (prev_pcb->current_esp) );
        asm volatile("movl %%ebp, %0" : "=r" (prev_pcb->current_ebp) );
    }

    asm volatile("movl %%esp, %0" : "=r" (pcb->parent_esp) );
    asm volatile("movl %%ebp, %0" : "=r" (pcb->parent_ebp) );
    asm volatile("movl %0, %%ebp" :: "r" (tss.esp0) );
    asm volatile("movl %0, %%esp" :: "r" (tss.esp0) );
    back_to_user_mode(entry_point);

    return 0;
}


/*open a file
input: filename
output: i = file index, -1 if fail*/
int32_t open(const uint8_t* filename)
{
    dentry_t temp;
    int i;
    if( read_dentry_by_name ( filename, &temp ) == -1 ) return -1;
    pcb_t* pcb;
    pcb = (pcb_t*) get_current_pcb();
    for(i = 0; i<FDT_SIZE; i++)
        if( (pcb->file_array)[i].flags == 0) break;

    if(i==FDT_SIZE) return -1; //no available FDT!

    (pcb->available)[i] = 1;
    (pcb->file_array)[i].inode = temp.inode;
    (pcb->file_array)[i].flags = 1;

    (pcb->file_array)[i].file_position= 0;
    if( temp.fileType == RTC_TYPE)
    {
        (pcb->file_array)[i].read = rtc_read_wrapper;
        (pcb->file_array)[i].write = rtc_write_wrapper;
    }
    else if( temp.fileType == REG_TYPE)
    {
        (pcb->file_array)[i].read = filesys_read;
        (pcb->file_array)[i].write = filesys_write_wrapper;
    }
    else if( temp.fileType == DIR_TYPE)
    {
        filesys_open();
        (pcb->file_array)[i].read = dir_read_wrapper;
        (pcb->file_array)[i].write = dir_write_wrapper;
    }

    return i;
}

/*read a file into buffer
Input: fd = file descriptor, buf = buffer to fill, nbytes = length
output: -1 if fail, bytes_read if successful, number of bytes read*/
int32_t read(int32_t fd, void* buf, int32_t nbytes)
{
    if( (fd >= FDT_SIZE) || (fd<0) || (fd==1)  ) return -1;
    else
    {
        pcb_t* pcb = (pcb_t*) get_current_pcb();
        if((pcb->file_array)[fd].flags==0) return -1;
        int offset = (pcb->file_array)[fd].file_position;
        int inode = (pcb->file_array)[fd].inode;
        int bytes_read = (pcb->file_array)[fd].read(inode, offset, buf, nbytes);
        if( bytes_read == -1 )
            return -1;

        (pcb->file_array)[fd].file_position += bytes_read;
        return bytes_read;

    }
}

/*writes a buffer into file
input: fd = file descriptor, buf = buffer to write from, nbytes = length
output: -1 if fail, bytes_read if successful, number of bytes write*/
int32_t write(int32_t fd, void* buf, int32_t nbytes)
{
    if( (fd >= FDT_SIZE) || (fd<0) || (fd==0) ) return -1;
    else
    {
        pcb_t* pcb = (pcb_t*) get_current_pcb();
        if((pcb->file_array)[fd].flags==0) return -1;
        int offset = 0;
        int inode = (pcb->file_array)[fd].inode;
        int bytes_written = (pcb->file_array)[fd].write(inode, offset, buf, nbytes);
        if( bytes_written == -1 )
            return -1;

        return bytes_written;

    }
}
/*get arguments from the command in execute
input: buf = part of command from execute, nbytes = length
output: -1 if fail, 0 if success*/
int32_t getargs(uint8_t* buf, int32_t nbytes)
{
    if(buf == NULL ) return -1;
    if(arg_available[current_pid]==0) return -1;
    memcpy(buf, arg_buf[current_pid], nbytes);
    return 0;
}

int32_t do_nothing()
{
    return 0;
}


/*close
input: fd = file descriptor
output: -1 if fail, 0 if success.*/

int32_t close(int32_t fd)
{
    pcb_t* pcb;
    pcb = (pcb_t*) get_current_pcb();
    if( (fd >= FDT_SIZE) || (fd<=1) || ((pcb->file_array)[fd].flags==0) ) return -1;
    (pcb->file_array)[fd].flags = 0;
    return 0;
}

//set up vidmap start address
//input: screenstart = a pointer to the starting address of the viemap
//output: -1 if fail, 0 if success.
int32_t vidmap(uint8_t **screenstart)
{
    if(screenstart == NULL) return -1; //check null input
    if( ( (int)(screenstart) < USER_PAGE_START ) || ( (int)(screenstart) >= USER_PAGE_END) ) return -1; //check memory range
    *screenstart = (uint8_t*) (VIDEO_USER);
    return 0;
}
