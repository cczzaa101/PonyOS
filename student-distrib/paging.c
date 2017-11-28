#include "x86_desc.h"
#include "lib.h"
#include "paging.h"
/* credit to http://wiki.osdev.org/Setting_Up_Paging */
#define kernel_dir page_dir[1]
#define initial_dir page_dir[0]
#define SINGLE_PAGE_DIR_OFFSET 1024
#define USER_LEVEL_OFFSET  (2*1024)
#define USER_SPACE_START 32
#define USER_VIDEO_START 40
/*Initialize page table's entry to all 0
 *Input: ind = the index of entry
 *output: none
 */
void page_table_init(int ind)
{
    #define t(i) page_table[i]
    t(ind).present = 0;
    t(ind).read_write =0;
    t(ind).user_supervisor=0;
    t(ind). write_through =0;
    t(ind). cache_disabled=0;
    t(ind). accessed   =0;
    t(ind). dirty      =0;
    t(ind). zero       =0;
    t(ind). global     =0;
    t(ind). avail      =0;
    t(ind). page_address=0;
}

/*Initialize user video page table's entry to all 0
 *Input: ind = the index of entry
 *output: none
 */
void user_video_page_table_init(int ind)
{
    #define vt(i) user_video_page_table[i]
    vt(ind).present = 0;
    vt(ind).read_write =0;
    vt(ind).user_supervisor=0;
    vt(ind). write_through =0;
    vt(ind). cache_disabled=0;
    vt(ind). accessed   =0;
    vt(ind). dirty      =0;
    vt(ind). zero       =0;
    vt(ind). global     =0;
    vt(ind). avail      =0;
    vt(ind). page_address=0;
}


/*Initialize page directory's entry to all 0
 *Input: ind the index of page directory
 *output: none
 */
void page_directory_init(int ind)
{
    #define d(i) page_dir[i]
    d(ind).present = 0;
    d(ind).read_write =1;
    d(ind).user_supervisor=0;
    d(ind). write_through =0;
    d(ind). cache_disabled=0;
    d(ind). accessed   =0;
    d(ind). reserved   =0;
    d(ind). size       =0;
    d(ind). zero       =0;
    d(ind). avail      =0;
    d(ind). aligned_address=0;
}

//set up video paging 
//input,output£ºnone
 
void setupt_video_user_level()
{
    /* setup user video page table*/
    int i;
    for(i=0; i< PAGE_TABLE_SIZE; i++)
    {
        //first_page_table[i] = (i * 0x1000) | 3;
        // attributes: supervisor level, read/write, present.
        user_video_page_table_init(i);
        if(i!=0) user_video_page_table[i].present = 1; //reserved for null pointer
        user_video_page_table[i].user_supervisor = 1;
        user_video_page_table[i].read_write = 1;
        user_video_page_table[i].page_address = i;
    }

    page_directory_init(USER_VIDEO_START);
    page_dir[USER_VIDEO_START].present = 1;
    page_dir[USER_VIDEO_START].user_supervisor = 1;
    page_dir[USER_VIDEO_START].size = 0; //4kb page
    page_dir[USER_VIDEO_START].aligned_address = ((int)user_video_page_table)>>12; //clear the last 12 bits to ensure alignment

}
/*initialize paging
input, output: None
effets: initialize kernel and first page directory entry, also the first page table
*/
void paging_init()
{
    int i;
    /* un used page dir starting from index 2 */
    for(i = 2; i< PAGE_DIRECTORY_SIZE; i++)
    {
        // present set to 0
        // r/w enable
        // supervisor mode
        // 4kb page
        page_directory_init(i);
        page_dir[i].read_write = 1;
        page_dir[i].aligned_address = i*1024; //aligned by 4mb = 1024*4kb
        page_dir[i].size = 1;
    }

    /* setup first page table*/
    for(i=0; i< PAGE_TABLE_SIZE; i++)
    {
        //first_page_table[i] = (i * 0x1000) | 3;
        // attributes: supervisor level, read/write, present.
        page_table_init(i);
        if(i!=0) page_table[i].present = 1; //reserved for null pointer
        page_table[i].read_write = 1;
        page_table[i].page_address = i;
    }

    page_directory_init(0);
    initial_dir.aligned_address = ((int)page_table)>>12; //clear the last 12 bits to ensure alignment
    initial_dir.present = 1;

    page_directory_init(1);
    kernel_dir.present = 1;
    kernel_dir.aligned_address = 1024; //start at 4mb = 1024*4kb
    kernel_dir.size = 1; //4mb page
    kernel_dir.user_supervisor = 1;

    setupt_video_user_level();

    //1. page_dir address to cr3
    //2. set paging and protection bits of cr0
    //3. set cr4 to enable 4bits
    //0x00000010 for enabling 4mb size
    //0x80000001 for enabling protected mode and paging mode

    __asm__ ( "leal page_dir,%eax;"
              "movl %eax,%cr3;"

              "movl %cr4, %eax;"
              "orl $0x00000010, %eax;"
              "movl %eax, %cr4;"

              "movl %cr0,%eax;"
              "orl $0x80000001, %eax;"
              "movl %eax,%cr0;"
    );
}

/*setting up task page
input: ind= index of pages
output: none*/
void setup_task_page(int ind)
{
    page_directory_init(USER_SPACE_START);
    page_dir[USER_SPACE_START].present = 1;
    page_dir[USER_SPACE_START].user_supervisor = 1;
    page_dir[USER_SPACE_START].size = 1; //4mb page? not sure
    page_dir[USER_SPACE_START].aligned_address = (ind * SINGLE_PAGE_DIR_OFFSET) + USER_LEVEL_OFFSET; //start at 8mb + ind*4mb
    __asm__ ( "leal page_dir,%eax;"
              "movl %eax,%cr3;"

              "movl %cr4, %eax;"
              "orl $0x00000010, %eax;"
              "movl %eax, %cr4;"

              "movl %cr0,%eax;"
              "orl $0x80000001, %eax;"
              "movl %eax,%cr0;"
    );
}
