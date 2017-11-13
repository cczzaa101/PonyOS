#ifndef _SYSCALL_H
#define _SYSCALL_H
#include "fileSystem.h"
/*
typedef struct{
    pcb_t * parent;

} pcb_t;
*/
int32_t execute (const uint8_t* command);

#endif
