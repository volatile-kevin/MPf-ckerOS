#include "types.h"
#include "i8259.h"
#include "lib.h"

/*init_rtc()
 * Initialize the rtc to interrupt at 1024 Hz
 * output data to the 
 */ 
void init_rtc();
/*rtc_handler()
*  handles the interrupt, "resets" so that interrupts can continue
*  also includes a test and sends EOI back
*/
void rtc_handler();

/**rtc_open
* turns on rtc, initializing frequency to 2hz
* @param: none
* @return 0 for success, -1 for failure
*/
void rtc_open();

/**rtc_close
* return 0, can be used for virtualization
*
*/
void rtc_close();

/**rtc_read
* function to block (wait) until the next RTC interrupt
* hmm idk how we will do this, lets see
*/
char rtc_read();

/**rtc_write
* change the frequency, must be a multiple of 2
*/
void rtc_write(unsigned int freq);



