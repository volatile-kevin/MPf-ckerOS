#ifndef SYSCALL_H
#define SYSCALL_H

#include "types.h"

/*
syscall.h and syscall.c is called from our syscall_asm.S file with the parameters pushed from the 0x80 user-level interrupt.
All parameters are defined and pushed to these functions within the syscall_asm.S file, and now all we have to do here in syscall.c and syscall.h
is interpret these parameters and use them in the kernel level code.
*/

extern void syscall_handler();

extern int32_t sys_halt (uint8_t status);

extern int32_t sys_execute (const uint8_t* command);

extern int32_t sys_read (int32_t fd, void* buf, int32_t nbytes);

extern int32_t sys_write (int32_t fd, const void* buf, int32_t nbytes);

extern int32_t sys_open (const uint8_t* filename);

extern int32_t sys_close (int32_t fd);

extern int32_t sys_getargs(uint8_t* buf, int32_t nbytes);

#endif
