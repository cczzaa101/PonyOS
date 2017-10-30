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


int32_t read_dentry_by_name (const uint8_t * fname, dentry_t *dentry)
{
    int i = 1, cmpRes;  
    for(i = 0; i<num_dir_entry; i++)
    {
        if(strlen( (int8_t *)fname ) != strlen(dir_table[i].fileName)) continue;
        cmpRes = strncmp( (int8_t *)fname, dir_table[i].fileName , strlen( (int8_t *)fname) );
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

int32_t read_dentry_by_index (uint32_t index, dentry_t *dentry)
{
    if( (index >= num_dir_entry) || (index<0) ) return -1;
    dentry->inode = dir_table[index].inode;
    dentry->fileType = dir_table[index].fileType;
    memcpy( dentry->reserved, dir_table[index].reserved, sizeof(dir_table[index].reserved) );
    memcpy( dentry->fileName, dir_table[index].fileName, sizeof(dir_table[index].fileName) );
    return 0;
}

int32_t read_data (uint32_t inode, uint32_t offset, uint8_t* buf, uint32_t length)
{
    int i, block_ind_offset, first_block_offset;
    int remaining_length = length, copied_length;
    uint8_t* current_buf_addr = buf;
    
    if( (inode<0) || (inode>=num_inodes) ) return -1; //invalid index
    if ( (offset + length) > inode_table[inode].length ) return -1; //invalid size
    
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
                        (uint8_t*)(inode_table[inode].data[i] + first_block_offset), 
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
                        (uint8_t*)(inode_table[inode].data[i]), 
                        copied_length
                      );
            remaining_length -= copied_length;
            current_buf_addr += copied_length;
        }
        
        if(remaining_length<=0) break;
    }
    
    return 0;
}

int32_t filesys_read(uint32_t inode, uint32_t offset, uint8_t* buf, uint32_t count)
{
    return read_data(inode, offset, buf, count);
}

int32_t filesys_write()
{
    return -1;
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

int32_t dir_read(char * buf)
{
    dentry_t temp;
    int res = read_dentry_by_index(current_dir_read_index,&temp);
    if(res == -1) return res;
    memcpy( buf, temp.fileName, sizeof(temp.fileName) );
    return 0;
}