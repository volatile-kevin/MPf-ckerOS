#include "schedule.h"


#define PIT0 0x40         
#define PIT1 0x41
#define PIT2 0x42
#define PIT_CMD 0x43
#define MAX_FREQ_PIT 1193180
#define CMD_BYTE 0x36
#define PIT_IRQ 0
#define BYTESHIFT 8
#define SPEAKERPORT 0x61
#define MASK 3

volatile uint8_t pitCount = 0;

// makes beep!
// take in frequency that you want the computer to play
// returns nothing
// side effects: Enables PIT interrupts
void beep(int frequency)
{
    uint32_t divisor = MAX_FREQ_PIT / frequency;
    outb(0xB6, PIT_CMD);
    outb((unsigned char) (divisor), PIT2);
    outb((unsigned char) (divisor >> BYTESHIFT), PIT2);

    unsigned char tempA = inb(SPEAKERPORT);

    if (tempA != (tempA | MASK)) 
    {
            outb(tempA | MASK, SPEAKERPORT);
    }

    enable_irq(PIT_IRQ); //tell the PIC to enable this interrupt

}

// initialize the PIT device
// takes in frequency of interrupts
// returns nothing
// side effects: enables PIT interrupts at 10 - 50ms 
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
// handles the PIT interrupts
// takes in nothing
// return nothing
// keeps a counter of amount of interrupts
// should spawn new shells at 0, 1, 2 interrupts
void pit_handler(){
    send_eoi(PIT_IRQ);
    //write to register C so that the interrupts can continously trigger
    // outb(REGC, PIT0);
    // // get rid of its data
    // inb(PIT_CMD);

    test_interrupts();

    if(pitCount == 0 || pitCount == 1 || pitCount == 2){
        pitCount++;
        spawn_shells(pitCount);
    }
}

void spawn_shells(int pid){

    if (pitCount < 3)
        execute((uint8_t*)"shell", 0);
    else
        execute((uint8_t*)"shell", 1);

    printf("pit count:%d\n", pitCount);
    //while (1);
}

void switch_terminal(uint8_t terminal){
    // copy the video memory into the current buffer??
    // copy the new buffer into video memory
    //
    return;
}

