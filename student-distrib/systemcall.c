#include "lib.h"
#include "systemcall.h"
#include "keyboard.h"
#define MAX_ARG_SIZE 4096
#define USER_PROGRAM_ADDR 0x8048000
#define ASSIGNED_PCB_SIZE 0x2000
char arg_buf[MAX_ARG_SIZE];
char process_status = "\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0";
static int arg_available = 0;
static int kernel_stack_bottom = (0x0800000)

void pcb_init(int pid)
{
    int i;
    kernel_stack_bottom -= ASSIGNED_PCB_SIZE
    (pcb_t*) pcb = (pcb_t*) kernel_stack_bottom;
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
    pcb->parent = kernel_stack_bottom + ASSIGNED_PCB_SIZE;
}

int32_t get_empty_pid()
{
    int i;
    for(i=0; i<16; i++)
    {
        if( process_status[i] == 0 )
            return i;
    }
    return -1;
}

int32_t execute (const uint8_t* command)
{
    int i,j,cnt = 0;
    char fname[MAX_ARG_SIZE];

    for(i=0; i<strlen( (char*)command); i++)
        if( command[i] == ' ' ) break;

    memcpy(fname, command, i);
    fname[ i ] = '\0';

    dentry_t f_entry;
    if( read_dentry_by_name((unsigned char*)fname, &f_entry) == -1 ) return -1; //doesn't exist, return -1

    /* check if executable */
    char executable_checking[4] = { 0x7f, 0x45, 0x4c, 0x46 };
    char tempBuf[MAX_ARG_SIZE];
    filesys_read_by_name((unsigned char*)fname, (unsigned char*)tempBuf, 28);
    for(j=0; j<4; j++)
    {
        if(tempBuf[j]!= executable_checking[j])
            return -1;
    }

    /* load user_level program */
    if(load_executable( fname, (char*)USER_PROGRAM_ADDR )==-1) return -1;

    /*make a note of entry point */
    int32_t * tempBuf_int = (int32_t *) tempBuf;
    int32_t entry_point = tempBuf[7]; //at bytes 23~27, i.e. 28/4 = 7 for int

    /* copy argument */
    for(;i<strlen( (char*)command); i++ )
        arg_buf[cnt++] = command[i];
    arg_buf[cnt] = '\0';
    arg_available = 1;

    /* assign PCB */
    int pid = get_empty_pid();
    process_status[pid] = 1;
    pcb_init(pid);

    /* assign page */
    setup_task_page(pid);

    return 0;
}

int32_t open(const uint8_t* filename)
{
    dentry_t temp;
    int i;
    if( read_dentry_by_name ( fname, &temp ) == -1 ) return -1;
    (pcb_t*) pcb = (pcb_t*) kernel_stack_bottom;
    for(i = 0; i<FDT_SIZE; i++)
        if( (pcb->file_array)[i].flags == 0) break;

    if(i==FDT_SIZE) return -1; //no available FDT!

    (pcb->available)[i] = 1;
    (pcb->file_array)[i].inode = temp.inode;
    (pcb->file_array)[i].flags = 1;

    (pcb->file_array)[i].file_position= 0;
    if(pcb->fileType == RTC_TYPE)
    {
        (pcb->file_array)[i].read = RTC_read_wrapper;
        (pcb->file_array)[i].write = RTC_write_wrapper;
    }
    else if(pcb->fileType == REG_TYPE)
    {
        (pcb->file_array)[i].read = filesys_read;
        (pcb->file_array)[i].write = filesys_write_wrapper;
    }
    else if(pcb->fileType == DIR_TYPE)
    {
        (pcb->file_array)[i].read = DIR_read_wrapper;
        (pcb->file_array)[i].write = DIR_write_wrapper;
    }

    return 0;
}

int32_t read(int32_t fd, void* buf, int32_t nbytes)
{
    if(fd >= FDT_SIZE ) return -1;
    else
    {
        (pcb_t*) pcb = kernel_stack_bottom;
        int offset = (pcb->file_array)[fd].file_position;

        int bytes_read = (pcb->file_array)[fd].read(inode, offset, buf, nbytes);
        if( bytes_read == -1 )
            return -1;

        (pcb->file_array)[fd].file_position += bytes_read;
        return bytes_read;

    }
}

int32_t write(int32_t fd, void* buf, int32_t nbytes)
{
    if(fd >= FDT_SIZE ) return -1;
    else
    {
        (pcb_t*) pcb = kernel_stack_bottom;
        int offset = 0;

        int bytes_written = (pcb->file_array)[fd].write(inode, offset, buf, nbytes);
        if( bytes_written == -1 )
            return -1;

        return bytes_written;

    }
}

int32_t getargs(uint8_t* buf, int32_t nbytes)
{
    if(buf == NULL ) return -1;
    if(arg_available==0) return -1;
    copy_to_user(buf, arg_buf, nbytes);
    return 0;
}
