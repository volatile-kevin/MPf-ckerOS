#include "idt.h"
#include "x86_desc.h"
#include "keyboard.h"
#include "lib.h"
#include "idt.h"
#include "rtc.h"
#include "schedule.h"
// calls our keyboard_handler.c function, saving all registers
extern void keyboard_asm();

//calls our rtc handler and saves reggies
extern void rtc_asm();
extern void sys_call_asm();
extern void pit_asm();
