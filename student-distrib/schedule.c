#include "pcb.h"
#include "execute.h"
#include "halt.h"
#include "lib.h"
#include "types.h"
#include "rtc.h"


#define PIT0 0x40         
#define PIT1 0x41
#define PIT2 0x42
#define PIT_CMD 0x43
#define MAX_FREQ_PIT 1193180
#define CMD_BYTE 0x36
#define PIT_IRQ 0

volatile uint8_t pitCount = 0;

void beep(int frequency)
{
    uint32_t divisor = 1193180 / frequency;
    outb(0xB6, PIT_CMD);
    outb((unsigned char) (divisor), PIT2);
    outb((unsigned char) (divisor >> 8), PIT2);

    unsigned char tempA = inb(0x61);

    if (tempA != (tempA | 3)) 
    {
            outb(tempA | 3, 0x61);
    }

    enable_irq(PIT_IRQ); //tell the PIC to enable this interrupt

}


void init_PIT(uint32_t frequency){

    uint32_t divisor = MAX_FREQ_PIT / frequency;

    outb(CMD_BYTE, PIT_CMD);

    uint8_t low = (uint8_t)(divisor & 0xFF);
    uint8_t high = (uint8_t)((divisor>>8) & 0xFF);

   // Send the frequency divisor.
    outb(low, PIT0);
    outb(high, PIT0);

    enable_irq(PIT_IRQ); //tell the PIC to enable this interrupt

}

void pit_handler(){
    send_eoi(PIT_IRQ);
    cli();
    //write to register C so that the interrupts can continously trigger
    // outb(REGC, PIT0);
    // // get rid of its data
    // inb(PIT_CMD);

    //test
    test_interrupts();

    if(pitCount == 0 || pitCount == 1 || pitCount == 2){
        // sti();
        pitCount++;
        execute((uint8_t*)"shell");
    }
}



