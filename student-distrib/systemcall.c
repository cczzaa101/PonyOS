#include "lib.h"
#include "systemcall.h"
#define MAX_ARG_SIZE 4096
#define USER_PROGRAM_ADDR 0x8048000
char arg_buf[MAX_ARG_SIZE];
static int arg_available = 0;
int32_t execute (const uint8_t* command)
{
    int i,j;
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

    if(load_executable( fname, (char*)USER_PROGRAM_ADDR )==-1) return -1;

    return 0;
}
