#include "idt2.h"
#include "x86_desc.h"
#include "keyboard.h"
#include "lib.h"
#include "idt2.h"
#include "rtc.h"

// calls our keyboard_handler.c function, saving all registers
extern void keyboard_asm();

//calls our rtc handler and saves reggies
extern void rtc_asm();
