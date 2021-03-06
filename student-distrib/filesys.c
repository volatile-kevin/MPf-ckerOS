#include "filesys.h"
#include "multiboot.h"
#include "lib.h"
#include "tests.h"
#include "types.h"
#include "pcb.h"

#define INODES_OFFSET 1
#define NUMBLOCKS_OFFSET 2
#define SIZEOF_DENTRY 64
#define NUM_DENTRIES 64
#define DATABLOCK_SIZE 4096
#define MAXFILENAMESIZE 32
#define FILENAMESIZE_NUL 31
#define SIZE_FDT 8
// global variables
uint8_t *start_addr;
uint32_t numdentries;
uint32_t numinodes;
uint32_t numblocks;
dentry_t *startdentry;
uint32_t curfile = 1;
file_desc_t *fdt[SIZE_FDT];
dentry_t currDentry;
int bytescounter = 0;

// this function initializes the file filesystem
// it reads the boot block and fills global variables with the boot block meta data
// the only param is the address of the bootblock in memory and is void
void filesys_init(uint32_t *mod_start) {
    start_addr = (uint8_t *) mod_start;
    numdentries = *mod_start;
    numinodes = *(mod_start + INODES_OFFSET);
    numblocks = *(mod_start + NUMBLOCKS_OFFSET);
    startdentry = (dentry_t *) (start_addr + SIZEOF_DENTRY);
    curfile = 1;
}

int32_t file_load(void *buf, int32_t nbytes) {
    int32_t bytes_copied = read_data(currDentry.inodeNum, 0, buf, nbytes);
    return bytes_copied;
}

// this function reads a file by calling read_data
// its parameters get passed into read_data, and the offset param for read_data is 0 for now
// MUST CALL FILE_OPEN BEFORE CALLING THIS
int32_t file_read(int32_t fd, void *buf, int32_t nbytes) {
    PCB_struct *curPCB = get_current_PCB();
    int32_t bytes_copied;
    if (curPCB != NULL && fd > -1) {
        bytes_copied = read_data(curPCB->fd_table[fd].inode_number, curPCB->fd_table[fd].file_pos, buf, nbytes);
        if (bytes_copied != 0)
            curPCB->fd_table[fd].file_pos += bytes_copied;
        else
            curPCB->fd_table[fd].file_pos = 0;
    } else {
        bytes_copied = read_data(currDentry.inodeNum, 0, buf, nbytes);
    }
    return bytes_copied;
}

/**
 * file_write
 * @return -1
 * Nothing to initialize
 */
int file_write(int32_t fd, const void *buf, int32_t nbytes) {
    (void) fd;
    (void) nbytes;
    (void) buf;
    return -1;
}

/**
 * file_read
 * @return 0
 * Nothing to tear down
 */
int file_close(int32_t fd) {
    (void) fd;
    return -1;
}

// this function prepares a file to be read
// it fills a global dentry struct that file_read will use
// in this case, we are reading by name
int file_open(const uint8_t *filename) {
    int32_t retval = read_dentry_by_name(filename, &currDentry);
    return retval;
}

// this function uses read_dentry_by_index in order to get all files in a directory
// for now, it acts as an "ls"
// we use strncpy to get the names of all the files
int32_t dir_read(int32_t fd, void *buf, int32_t nbytes) {
    (void) fd;
    (void) nbytes;
    uint8_t *fname;
    fname = (uint8_t *) buf;
    int i;
    for (i = 0; i < MAXFILENAMESIZE; i++) {
        fname[i] = 0;
    }
    dentry_t dentry;
    int32_t retval = read_dentry_by_index(curfile, &dentry);
    strncpy((int8_t *) fname, (int8_t *) dentry.name, nbytes);
    curfile++;
    if (curfile == numdentries) {
        curfile = 0;
        return 0;
    } else {
        return retval;
    }
}

/**
 * dir_write
 * @return -1
 * We have nothing to initialize
 */
int dir_write(int32_t fd, const void *buf, int32_t nbytes) {
    (void) fd;
    (void) nbytes;
    (void) buf;
    return -1;
}

/**
 * dir_read
 * @return 0
 * We have nothing to tear down
 */
int dir_close(int32_t fd) {
    (void) fd;
    return -1;
}

// this function prepares a directory to be read
// it fills a global dentry struct that file_read will use
// in this case, we are reading by name
int dir_open(const uint8_t *filename) {
    int32_t retval = read_dentry_by_name(filename, &currDentry);
    return retval;
}

// this function reads directory entries by their names
// we must loop through all of the dentries to find a match
// params are a file name a dentry
// we will eventually copy the file name into the dentry struct passed in
int32_t read_dentry_by_name(const uint8_t *fname, dentry_t *dentry) {
    unsigned dentIndex = 0;
    unsigned nameIndex = 0;
    dentry_t *temp = startdentry;

    while (dentIndex < NUM_DENTRIES) {
        temp = startdentry + dentIndex;
        unsigned strLength = FILENAMESIZE_NUL;
        unsigned count = 0;
        while (temp->name[nameIndex] == fname[nameIndex]) {
            if (temp->name[nameIndex] == '\0' || count == strLength) {
                unsigned i = 0;

                for (; i < NAME_SIZE; i++) //deep copy the file name
                    dentry->name[i] = temp->name[i];

                dentry->fileType = temp->fileType;
                dentry->inodeNum = temp->inodeNum;
                return 0;
            } // if we reach null char, this is the right string
            count++;
            nameIndex++;
        }
        dentIndex++;
        nameIndex = 0;
    }
    return -1;
}

// this function reads directory entries by their indecies
// we just have to add an offset which is index from the first dentry
// params are an index and a dentry
// we will eventually copy the file name into the dentry struct passed in
int32_t read_dentry_by_index(uint32_t index, dentry_t *dentry) {
    // return -1 on fail
    if (index >= NUM_DENTRIES || index < 0)
        return -1;

    // add index as an offset, cast the sum
    dentry_t *dentryptr = (dentry_t *) (startdentry + index);
    unsigned i = 0;
    for (; i < NAME_SIZE; i++) //deep copy the file name
        dentry->name[i] = dentryptr->name[i];
    dentry->fileType = dentryptr->fileType;
    dentry->inodeNum = dentryptr->inodeNum;

//    printf("%d",strlen((int8_t*)dentry->name));
//    return strlen((int8_t*)dentry->name);
    return MAXFILENAMESIZE < strlen((int8_t *) dentry->name) ? MAXFILENAMESIZE : strlen((int8_t *) dentry->name);
}

/* inode points to data
   offset is the number of bytes to skip into the file/data
   buf is what you copy into
   number of bytes to read*/
// this function reads data from files by accessing their data blocks
// we consider edge cases where offsets might bleed into separate data blocks etc.
// we copy the data into the parameter buf, up to length bytes
// we must first access the inode which tells us which data blocks hold the data
int32_t read_data(uint32_t inode, uint32_t offset, uint8_t *buf, uint32_t length) {
    // ??? create a dentry, pass into read_dentry_by_name or read_dentry_by_index
    // access inodeNum inside of dentry
    // use inode struct to access data blocks that hold file information
    // loop through the file data and put it into a buffer
    // until length in B is reached
//   uint32_t saveLength = length;
    // fail
    if (inode > numinodes || inode < 0) {
        return -1;
    }
    // set inodeBlock to the corresponding block in memory
    // +1 skips the boot block
    inode_t *inodeBlock = (inode_t *) (start_addr) + inode + 1;
    // set currDataBlock to the very first data block in memory
    // +1 skips the boot block
    dataBlock_t *currDataBlock = (dataBlock_t *) (start_addr) + numinodes + 1;

    uint32_t eofLength = inodeBlock->length;
    uint32_t eofCount = offset;

    // keep copying until length is 0
    while (length != 0 && eofLength > eofCount) {
        currDataBlock = (dataBlock_t *) (start_addr) + numinodes + 1;
        currDataBlock = (dataBlock_t *) (currDataBlock) + inodeBlock->dataBlockNums[eofCount / DATABLOCK_SIZE];
        uint32_t tempOffset = 0;
        uint32_t currLength = 0;//current amount you can copy from a block of data

        //only copy one datablock at a time
        if (length % DATABLOCK_SIZE == 0 || length > DATABLOCK_SIZE) {
            currLength = DATABLOCK_SIZE;
        }
            //make sure you don't go over the inode length
        else if (length + offset > eofLength) {
            currLength = eofLength - offset;
        } else {
            currLength = length;
        }
        tempOffset = offset % DATABLOCK_SIZE;
        uint8_t *position = (uint8_t *) (currDataBlock) + tempOffset;
        memcpy((void *) buf, (void *) (position), currLength);

        length -= currLength;
        eofCount += currLength;
        buf += currLength;
    }
    if (eofLength <= offset) {
        return 0;
    }
    return eofCount - offset;
}
