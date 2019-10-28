#include "rtc.h"

#define RTC_DATA 0x70
#define RTC_CMD  0x71
#define NMI      0x8B
#define REGBIT   0x40
#define REGC     0x0C
#define IRQ_RTC 8

unsigned int freq = 32768;
unsigned int rate;
uint8_t waiting = 0;


/*init_rtc()
 * Initialize the rtc to interrupt at 1024 Hz
 * output data to the 
 */
void init_rtc(){
    outb(NMI, RTC_DATA);            // select register B, and disable NMI
    char prev = inb(RTC_CMD);	    // read the current value of register B
    outb(NMI, RTC_DATA);	    // set the index again (a read will reset the index to register D)
    outb((prev | REGBIT), RTC_CMD); // write the previous value ORed with 0x40. This turns on bit 6 of register B

   // rtc_write(rate);

    enable_irq(IRQ_RTC); //tell the PIC to enable this interrupt 
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
    test_interrupts();

    sti();
    //send the EOI
    send_eoi(IRQ_RTC);
	waiting = 0;
   
}

/*assume cli and sti before/after this call*/
void rtc_open(){
	rate = 2;
	init_rtc();
	rtc_write(rate);
	//init_rtc();

}

/*block until the next interrupt*/
char rtc_read(){
	waiting = 1;
	while (waiting);
		return 0;
}

/*sets the frequency, must be a multiple of 2 [2,8192]*/
void rtc_write(unsigned int input_freq){
	rate = 0;
	while (input_freq <= 16384){
		input_freq <<= 1;
		rate++;
	}
	if (rate < 2)
		rate = 2;
	else if (rate > 15)
		rate = 15;

	//printf("RATE: %d\n", rate);

	rate &= 0x0F;
	outb(0x8A, RTC_DATA); //set index to register A, disable NMI
	char prev = inb(RTC_CMD);
	outb(0x8A, RTC_DATA); //set index to register A, disable NMI
	outb(((prev & 0xF0) | rate), RTC_CMD); // write the rate to RTC
}


