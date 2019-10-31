#include "types.h"

#define NAME_SIZE 32
#define NUMDATABLOX 1023
#define DENTRY_RESERVED 24
#define SIZE_DATABLOCK 1024
typedef struct __attribute__ ((packed)) {
  uint32_t length;
  uint32_t dataBlockNums[NUMDATABLOX];
} inode_t;

typedef struct __attribute__ ((packed)) {
  uint8_t name[NAME_SIZE];
  uint32_t fileType;
  uint32_t inodeNum;
  uint8_t reserved[DENTRY_RESERVED];
} dentry_t;

typedef struct __attribute__ ((packed)) {
  uint32_t data[SIZE_DATABLOCK];
} dataBlock_t;

void filesys_init(uint32_t* mod_start);

int file_read(int32_t fd, void* buf, int32_t nbytes);
int file_write();
int file_close();
int file_open(const uint8_t* filename);

int dir_read(uint8_t * fname);
int dir_write();
int dir_close();
int dir_open(const uint8_t* filename);

int32_t read_dentry_by_name(const uint8_t* fname, dentry_t* dentry);
int32_t read_dentry_by_index (uint32_t index, dentry_t* dentry);
int32_t read_data (uint32_t inode, uint32_t offset, uint8_t* buf, uint32_t length);