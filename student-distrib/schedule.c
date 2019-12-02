#include "pcb.h"
#include "execute.h"
#include "halt.h"
#include "lib.h"
#include "types.h"

#define PIT0 0x40         
#define PIT1 0x41
#define PIT2 0x42
#define PIT_CMD 0x43
#define MAX_FREQ 1193180
#define CMD_BYTE 0x36



void init_PIT(uint32_t frequency){

    uint32_t divisor = MAX_FREQ / frequency;

    outb(CMD_BYTE, PIT_CMD);
    uint8_t low = (uint8_t)(divisor & 0xFF);
    uint8_t high = (uint8_t)((divisor>>8) & 0xFF);

   // Send the frequency divisor.
    outb(0x40, low);
    outb(0x40, high);
}

