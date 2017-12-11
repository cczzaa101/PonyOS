#ifndef _SYSCALL_H
#define _SYSCALL_H
#include "fileSystem.h"
#include "keyboard.h"
#define FDT_SIZE 8
#define RTC_TYPE 0
#define DIR_TYPE 1
#define REG_TYPE 2
#define VIDEO_USER       (0xB8000+0xA000000) //user leve remapping
#define MAX_ARG_SIZE 4096
#define USER_PROGRAM_ADDR 0x8048000
#define USER_PAGE_START 0x8000000
#define USER_PAGE_END 0x8400000
#define ASSIGNED_PCB_SIZE 0x2000
#define EXEC_INFO_BYTES 28
#define MAX_PID 32
#define MEM_DEFENSE_SIZE 8
//int32_t (*file_operation[])(char*, int32_t) = { terminal_read, dir_read, filesys_read }
typedef struct{
    int32_t (*read) (uint32_t, uint32_t, uint8_t*, uint32_t);
    int32_t (*write) (uint32_t, uint32_t, uint8_t*, uint32_t);
    int32_t inode, file_position, flags;
}file_desc_t;

typedef struct{
    int32_t * parent;
    int32_t * child;
    int32_t parent_esp;//save for halt
    int32_t parent_ebp;
    int32_t current_esp;//save for scheduling
    int32_t current_ebp;
    file_desc_t file_array[FDT_SIZE];
    char available[FDT_SIZE];
    int32_t pid; //process id
} pcb_t;

int32_t execute (const uint8_t* command);
int32_t halt(int32_t status);
int32_t open(const uint8_t* filename);
int32_t read(int32_t fd, void* buf, int32_t nbytes);
int32_t write(int32_t fd, void* buf, int32_t nbytes);
int32_t getargs(uint8_t* buf, int32_t nbytes);
int32_t do_nothing();
int32_t close();
int32_t vidmap(uint8_t **sreenstart);
pcb_t* get_current_pcb();
int get_kernel_stack_bottom(int pid);
int is_running(int pid);
void set_current_pid(int pid);
int get_running_process_num();
#endif
