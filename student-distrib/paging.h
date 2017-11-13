#ifndef PAGING_H
#define PAGING_H
#include "x86_desc.h"
#include "lib.h"
/* paging initialization */
extern void paging_init();
void setup_task_page(int ind);
#endif
