#ifndef _SYSCALL_H
#define _SYSCALL_H
#include "fileSystem.h"
#include "keyboard.h"
#define FDT_SIZE 8
#define RTC_TYPE 0
#define DIR_TYPE 1
#define REG_TYPE 2
//int32_t (*file_operation[])(char*, int32_t) = { terminal_read, dir_read, filesys_read }
typedef struct{
    int32_t (*read) (uint32_t, uint32_t, uint8_t*, uint32_t);
    int32_t (*write) (uint32_t, uint32_t, uint8_t*, uint32_t);
    int32_t inode, file_position, flags;
}file_desc_t;

typedef struct{
    int32_t * parent;
    int32_t parent_esp;
    int32_t parent_ebp;
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

#endif
