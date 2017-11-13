#ifndef _SYSCALL_H
#define _SYSCALL_H
#include "fileSystem.h"
#include "keyboard.h"
#include "rtc.h"
#define FDT_SIZE 8
#define RTC_TYPE 0
#define DIR_TYPE 1
#define REG_TYPE 2
int32_t (*file_operation[])(char*, int32_t) = { terminal_read, dir_read, filesys_read }
typedef struct{
    int32_t (*operation) ();
    int32_t inode, file_position, flags;
}file_desc_t;

typedef struct{
    int32_t * parent;
    file_desc_t file_array[FDT_SIZE];
    char available[FDT_SIZE];
    int32_t pid; //process id
} pcb_t;

int32_t execute (const uint8_t* command);

#endif
