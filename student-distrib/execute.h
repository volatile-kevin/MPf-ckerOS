#pragma once
#include "types.h"

#define KERNEL_CS   0x0010
#define KERNEL_DS   0x0018
#define USER_CS     0x0023
#define USER_DS     0x002B
#define KERNEL_TSS  0x0030
#define KERNEL_LDT  0x0038

#define TSS_SIZE    104


void switch_to_user_mode();
int read_ELF(const uint8_t* fname, uint8_t* buf, int32_t fd);
// void fill_pcb_member(const uint8_t* fname);
int execute(const uint8_t* fname);
int load(const uint8_t* fname, uint8_t* buf);
// void switch_to_user_mode(void);
// void read_ELF(void);
// void execute(void);
