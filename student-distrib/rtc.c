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
    //test_interrupts();

    // sti();
    //send the EOI
    waiting = 0;

    send_eoi(IRQ_RTC);
}

/*rtc_open()
*  Initialize the rtc and set it to a default rate of 2 Hz
*/
int32_t rtc_open(const uint8_t* filename){
	(void) filename;

	int32_t freq = 2;
	init_rtc();

	rtc_write(0, &freq, 0);//start at 2 Hz
	return 0;
}

/*rtc_close()
 * close the rtc
 */
int32_t rtc_close(int32_t fd){
	(void) fd;
    int32_t buf[1];
    buf[0] = 0;
	rtc_write(0, &buf[0], 0);
	return 0;
}

/* rtc_read()
 * block until the next interrupt
 */
int32_t rtc_read(int32_t fd, void* buf, int32_t nbytes){
	(void) fd;
	(void) buf;
	(void) nbytes;

	waiting = 1; //flag variable to keep track of waiting
	while (waiting);
	return 0;
}

/* rtc_write()
 * sets the frequency, must be a multiple of 2 [2,8192]
 */
int32_t rtc_write(int32_t fd, const void* buf, int32_t nbytes){
    int32_t input_freq = *((int32_t*)buf);

    if(input_freq != 2 && input_freq != 4 && input_freq != 8
    && input_freq != 16 && input_freq != 32 && input_freq != 64
    && input_freq != 128 && input_freq != 256 && input_freq != 512
    && input_freq != 1024) {return -1;} //have to make sure that the input is a power of 2

    (void) fd;
	(void) nbytes;
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
	return 0;
}
