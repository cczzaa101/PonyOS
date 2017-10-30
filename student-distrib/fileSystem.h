#define inode_data_size 1023
#define fileNameLength 32
#define reserved_dentry_length 24
/* struct for file directory entry*/
typedef struct  {
    char fileName[fileNameLength];
    int32_t fileType;
    int32_t inode;
    char reserved[reserved_dentry_length];
} dentry_t;

typedef struct  {
    uint32_t length;
    uint32_t data[ inode_data_size ];
} inode_t;

void filesys_init();
int32_t read_dentry_by_name (const uint8_t * fname, dentry_t *dentry);
int32_t read_dentry_by_index (uint32_t index, dentry_t *dentry);
int32_t filesys_read(uint32_t inode, uint32_t offset, uint8_t* buf, uint32_t count);
int32_t filesys_write();
int32_t filesys_open();
int32_t filesys_close();
int32_t filesys_read_by_name(uint8_t* fname, uint8_t* buf, uint32_t count);
int32_t dir_read(char * buf);
int32_t dir_write();
int32_t dir_open();
int32_t dir_close();

