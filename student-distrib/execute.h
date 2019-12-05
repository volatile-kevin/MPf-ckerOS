#pragma once
#include "types.h"

#define KERNEL_CS   0x0010
#define KERNEL_DS   0x0018
#define USER_CS     0x0023
#define USER_DS     0x002B
#define KERNEL_TSS  0x0030
#define KERNEL_LDT  0x0038
#define HEADERSIZE 28
#define MAXBYTES 0x3B8000
#define HEADERBUFSIZE 100
#define NUMPROCESSES 6
#define VADDRPROGIMG 0x08048000
#define PIDOFFSET 2
#define VADDRPROGPAGE 0x08000000
#define USWFLAGS 0x87
#define KERNELSTACK 0x800000
#define EIGHTKB 0x2000
#define KSTACKOFFSET 4
#define MAXCOMMANDSIZE 128
#define TSS_SIZE    104


void switch_to_user_mode();
int read_ELF(const uint8_t* fname, uint8_t* buf, int32_t fd);
// void fill_pcb_member(const uint8_t* fname);
int execute(const uint8_t* fname, uint8_t switch_after_creation);
int load(const uint8_t* fname, uint8_t* buf);
// void switch_to_user_mode(void);
// void read_ELF(void);
// void execute(void);
