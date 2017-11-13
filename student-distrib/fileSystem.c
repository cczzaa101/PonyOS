#include "x86_desc.h"
#include "fileSystem.h"
#include "lib.h"
int32_t num_dir_entry;
int32_t num_inodes;
int32_t num_data_blocks;
int32_t current_dir_read_index;
char * data_block_start;
dentry_t * dir_table;
inode_t * inode_table;

#define dir_entry_offset 0
#define inodes_offset 4
#define data_blocks_offset 8
#define data_block_size (4*1024)
#define data_block(i) (data_block_start + data_block_size*i)
#define max_name_length 32
#define max_int 0x7fffffff

/*filesystem initialization*/
void filesys_init()
{
    int32_t * temp = (int32_t *) (file_start + dir_entry_offset);
    num_dir_entry = *temp;
    temp = (int32_t *) (file_start + inodes_offset);
    num_inodes = *temp;
    temp = (int32_t *) (file_start + data_blocks_offset);
    num_data_blocks = *temp;

    dir_table = (dentry_t *) ( file_start + sizeof(dentry_t) );

    inode_table = (inode_t *) ( file_start + sizeof(inode_t) );

    data_block_start = (char*) file_start + data_block_size * (num_inodes + 1);
}

/*Read directory entry by name
Input: const uint8_t * fname = file name string
       dentry_t *dentry = a dentry to be filled
Output: 0 if found , -1 fail
Side effect: find the file by its name and fill the dentry with the file name, file
type, and inode number for the file, then return 0*/
int32_t read_dentry_by_name (const uint8_t * fname, dentry_t *dentry)
{
    int i = 1,l1,l2, cmpRes;
    for(i = 0; i<num_dir_entry; i++)
    {
        l1 = strlen( (int8_t *)fname );
        l2 = strlen(dir_table[i].fileName);
        if(l1>max_name_length) l1 = max_name_length;
        if(l2>max_name_length) l2 = max_name_length;
        if( l1!=l2 ) continue;
        cmpRes = strncmp( (int8_t *)fname, dir_table[i].fileName , l1 );
        if(cmpRes == 0)
        {
            dentry->inode = dir_table[i].inode;
            dentry->fileType = dir_table[i].fileType;
            memcpy( dentry->reserved, dir_table[i].reserved, sizeof(dir_table[i].reserved) );
            memcpy( dentry->fileName, dir_table[i].fileName, sizeof(dir_table[i].fileName) );
            //break;
            return 0; //found! return 0
        }
    }
    return -1;
}

/*Read directory entry by name
Input: uint32_t index = file index
       dentry_t *dentry = a dentry to be filled
Output: 0 if found , -1 fail
Side effect: find the file by its index and fill the dentry with the file name, file
type, and inode number for the file, then return 0*/
int32_t read_dentry_by_index (uint32_t index, dentry_t *dentry)
{
    if( (index >= num_dir_entry) || (index<0) ) return -1;
    dentry->inode = dir_table[index].inode;
    dentry->fileType = dir_table[index].fileType;
    memcpy( dentry->reserved, dir_table[index].reserved, sizeof(dir_table[index].reserved) );
    memcpy( dentry->fileName, dir_table[index].fileName, sizeof(dir_table[index].fileName) );
    return 0;
}

/*Read data
  Input: uint32_t inode = Given node
		 uint32_t offset = the offset from starting position
		 uint8_t* buf = the buffer to be filled
		 uint32_t length = the length bytes to read
  Output: 0 if the end of the le has been reahed
  		-1 if fail
  Side Effect:  reading up to length bytes starting from position offset in the file with inode number inode and returning the number of bytes
				read and plaed in the buffer. */
int32_t read_data (uint32_t inode, uint32_t offset, uint8_t* buf, uint32_t length)
{
    int i, block_ind_offset, first_block_offset;
    int remaining_length = length, copied_length;
    uint8_t* current_buf_addr = buf;

    if( (inode<0) || (inode>=num_inodes) ) return -1; //invalid index
    if ( (offset + length) > inode_table[inode].length )
    {
        remaining_length = inode_table[inode].length - offset;
    }//invalid size

    block_ind_offset = offset / data_block_size;
    first_block_offset = offset % data_block_size;

    for( i = block_ind_offset; i < inode_table[inode].length; i++ )
    {
        if( i == block_ind_offset )
        {
            if( remaining_length >= (data_block_size - first_block_offset) )
                copied_length = (data_block_size - first_block_offset);
            else
                copied_length = remaining_length;

            memcpy(
                        current_buf_addr,
                        (uint8_t*)( data_block( inode_table[inode].data[i] ) + first_block_offset),
                        copied_length
                      );

            remaining_length -= copied_length;
            current_buf_addr += copied_length;

        }
        else
        {
            if( remaining_length >= data_block_size )
                copied_length = data_block_size;
            else
                copied_length = remaining_length;

            memcpy(
                        current_buf_addr,
                        (uint8_t*)( data_block( inode_table[inode].data[i] ) ),
                        copied_length
                      );
            remaining_length -= copied_length;
            current_buf_addr += copied_length;
        }

        if(remaining_length<=0) break;
    }

    return 0;
}

/*filesys read
  Input: uint32_t inode = Given node
		 uint32_t offset = the offset from starting position
		 uint8_t* buf = the buffer to be filled
		 uint32_t length = the length bytes to read
  Output: the return value of read_file
  Side effect: see read_data*/
int32_t filesys_read(uint32_t inode, uint32_t offset, uint8_t* buf, uint32_t count)
{
    return read_data(inode, offset, buf, count);
}

/*file system read by name
Input: const uint8_t * fname = file name string
       uint8_t* buf = buffer to be filled
	   uint32_t count = legnth of bytes
Output: see read_data
Side effect: find the inode of the file by its file name and read its content to the buffer up to "count" number*/
int32_t filesys_read_by_name(uint8_t* fname, uint8_t* buf, uint32_t count)
{
    dentry_t f;
    int res = read_dentry_by_name(fname, &f);
    if(res == -1) return -1;
    return read_data(f.inode, 0, buf, count);
}

int32_t filesys_write()
{
    return -1;
}

int32_t filesys_write_wrapper(uint32_t inode, uint32_t offset, uint8_t* buf, uint32_t count)
{
    return filesys_write();
}

int32_t filesys_open()
{
    return 0;
}

int32_t filesys_close()
{
    return 0;
}

int32_t dir_open()
{
    current_dir_read_index = 0;
    return 0;
}

int32_t dir_close()
{
    current_dir_read_index = 0;
    return 0;
}

int32_t dir_write()
{
    return -1;
}

int32_t dir_write_wrapper(uint32_t inode, uint32_t offset, uint8_t* buf, uint32_t count)
{
    return dir_write(buf);
}

/*directory read
 Input: char * buf = the buffer to read the name
 Output: 0 if there is file left to read name, -1 if no file left
 read and return a buffer with name of a file once a call and then move the index*/
int32_t dir_read(char * buf)
{
    if(current_dir_read_index >= num_dir_entry ) return -1;
    dentry_t temp;
    int res = read_dentry_by_index(current_dir_read_index,&temp);
    if(res == -1) return res;
    memcpy( buf, temp.fileName, sizeof(temp.fileName) );
    buf[max_name_length] = '\0';
    current_dir_read_index++;
    return strlen(buf);
}

int32_t dir_read_wrapper(uint32_t inode, uint32_t offset, uint8_t* buf, uint32_t count)
{
    return dir_read((char*)buf);
}

int32_t load_executable(char * fname, char* dest)
{
    if( filesys_read_by_name((uint8_t*)fname, (uint8_t*)dest, max_int)!= -1 )
        return 0;
    else return -1;
}
