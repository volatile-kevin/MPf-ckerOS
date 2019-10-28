#include "rtc.h"

#define RTC_DATA 0x70 //rtc data port
#define RTC_CMD  0x71 //rtc command port
#define NMI      0x80 //non maskable interrupt byte
#define REGBIT   0x40 //register bit of rtc
#define REGC     0x0C //Register C of RTC
#define IRQ_RTC  8 //IRQ line of RTC
#define MAX_FREQ 16384 //maximum frequency of the rtc (theoretically)
#define RTC_RATE_MAX 15 //1 hz is our minimum frequency
#define RTC_RATE_MIN 4 //1024 hz is our maximum frequency

unsigned int rate; //file scope variable to keep track of current rate
uint8_t waiting = 0; //variable that keeps track of if the rtc is blocked


/*init_rtc()
 * Initialize the rtc to interrupt at 1024 Hz
 * output data to the 
 */
void init_rtc(){
    outb(NMI | 0xB, RTC_DATA);            // select register B, and disable NMI
    char prev = inb(RTC_CMD);	    // read the current value of register B
    outb(NMI | 0xB, RTC_DATA);	    // set the index again (a read will reset the index to register D)
    outb((prev | REGBIT), RTC_CMD); // write the previous value ORed with 0x40. This turns on bit 6 of register B 

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

/*rtc_open()
*  Initialize the rtc and set it to a default rate of 2 Hz
*/
void rtc_open(){
	unsigned int freq = 2; //start at 2 Hz
	init_rtc();
	rtc_write(freq);
}

/*rtc_close()
 * close the rtc
 */ 
void rtc_close(){
	rtc_write(0);
}

/* rtc_read()
 * block until the next interrupt
 */
char rtc_read(){
	waiting = 1; //flag variable to keep track of waiting
	while (waiting);
	return 0;
}

/* rtc_write()
 * sets the frequency, must be a multiple of 2 [2,8192]
 */
void rtc_write(unsigned int input_freq){
	rate = 0; //reset rate

	if (input_freq){ //if input freq is not 0, 0 disables interrupt
		while (input_freq <= MAX_FREQ){
			input_freq <<= 1; //basically just count how long it takes to reach max frequency
			rate++;
		}
		if (rate < RTC_RATE_MIN)
			rate = RTC_RATE_MIN; //rate cant be less than 4 ( 1024 Hz)
		else if (rate > RTC_RATE_MAX)
			rate = RTC_RATE_MAX; //also cant be more than 15 
	}
	//printf("RATE1: %d\n", rate);

	rate &= 0x0F; //ensure it is less than 16
	outb(NMI | 0xA, RTC_DATA); //set index to register A, disable NMI
	char prev = inb(RTC_CMD);
	outb(0x8A, RTC_DATA); //set index to register A, disable NMI
	outb(((prev & 0xF0) | rate), RTC_CMD); // write the rate to RTC
}


