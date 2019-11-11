#include "idt2.h"
#include "x86_desc.h"
#include "keyboard.h"
#include "lib.h"
#include "rtc.h"
#include "syscall.h"

// takes in parameters and dispatches to each given function in syscall.c
extern void syscall_handler_asm();
