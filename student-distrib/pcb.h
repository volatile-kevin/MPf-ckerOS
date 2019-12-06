#ifndef PCB_H
#define PCB_H

#pragma once
#include "types.h"
#include "x86_desc.h"

#define NUMPCBS 6
#define NAME_SIZE 32
#define NUMDATABLOX 1023
#define DENTRY_RESERVED 24
#define SIZE_DATABLOCK 1024

#define INODES_OFFSET 1
#define NUMBLOCKS_OFFSET 2
#define SIZEOF_DENTRY 64
#define NUM_DENTRIES 64
#define DATABLOCK_SIZE 4096
#define MAXFILENAMESIZE 32
#define FILENAMESIZE_NUL 31
#define TEMPBUFSIZEH 1024



//#define FLAGS_SIZE 5 //size of flag array
#define NUM_FDT_ENTRIES 8 // number of entries in the PCB struct

typedef struct generic_struct{
    int32_t (*open)(const uint8_t* filename);
    int32_t (*close)(int32_t fd);
    int32_t (*read)(int32_t fd, void* buf, int32_t nbytes);
    int32_t (*write)(int32_t fd, const void* buf, int32_t nbytes);
} fop;

typedef struct fd_table {
    // making flag array with multiple entires (but should only need 1}
    // 1 marks in use, -1 marks free
    int present;
    int file_pos;
    int inode_number;
    // this will be what we use to index into the jump table of all the different
    // open, close, read, and write functions
    fop* jump_start_idx;
    int fileType;
} fd_table_entry_t;

typedef struct PCB_struct {
    fd_table_entry_t fd_table[NUM_FDT_ENTRIES];
    int pcb_in_use; // -1 for empty, 0 for in use
    int esp;
    int ebp;
    int esp0;
    int ss0;
    int return_label_add;
    int parent_pid;
    uint8_t args[TEMPBUFSIZEH];
} PCB_struct;




extern PCB_struct PCB_array[6];

extern int insert_fdt(const uint8_t* filename);

extern int getPID();

extern int get_fdAvail(int process_id);

void pcb_filesys_init(uint32_t* mod_start);

void init_PCB();

extern int insert_FDT_entry(uint8_t process_id, unsigned inodeNum, unsigned filePosition, unsigned flags);

int remove_fd_entry(int fd);

extern PCB_struct* get_current_PCB();

#endif
