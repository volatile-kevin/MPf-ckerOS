#include "rtc.h"

#define RTC_DATA 0x70
#define RTC_CMD 0x71
#define NMI 0x8B
#define REGBIT 0x40
#define REGC 0x0C
#define SIZEOFPIC1 8

/*init_rtc()
 * Initialize the rtc to interrupt at 1024 Hz
 * output data to the 
 */ 
void init_rtc(){
    
    outb(NMI, RTC_DATA);		// select register B, and disable NMI
    char prev = inb(RTC_CMD);	// read the current value of register B
    outb(NMI, RTC_DATA);		// set the index again (a read will reset the index to register D)
    outb((prev | REGBIT), RTC_CMD);	// write the previous value ORed with 0x40. This turns on bit 6 of register B

    enable_irq(SIZEOFPIC1);
}

/*rtc_handler()
*  handles the interrupt, "resets" so that interrupts can continue
*  also includes a test and sends EOI back
*/
void rtc_handler(){
    cli();
    //write to register C so that the interrupts can continously trigger
    outb(REGC, RTC_DATA);
    // get rid of its data
    inb(RTC_CMD);
    //test
//    test_interrupts();

    sti();
    //send the EOI
    send_eoi(SIZEOFPIC1);
   
}

