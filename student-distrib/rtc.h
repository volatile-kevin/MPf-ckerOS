#include "types.h"
#include "i8259.h"
#include "lib.h"
#include "types.h"

#define RTC_DATA 0x70 //rtc data port
#define RTC_CMD  0x71 //rtc command port
#define NMI      0x80 //non maskable interrupt byte
#define REGBIT   0x40 //register bit of rtc
#define REGC     0x0C //Register C of RTC
#define IRQ_RTC  8 //IRQ line of RTC
#define MAX_FREQ 16384 //maximum frequency of the rtc (theoretically)
#define RTC_RATE_MAX 15 //1 hz is our minimum frequency
#define RTC_RATE_MIN 4 //1024 hz is our maximum frequency
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

// int32_t (*open)(const uint8_t* filename);
int32_t rtc_open(const uint8_t* filename);

/**rtc_close
* return 0, can be used for virtualization
*
*/
int32_t rtc_close(int32_t fd);

/**rtc_read
* function to block (wait) until the next RTC interrupt
* hmm idk how we will do this, lets see
*/
int32_t rtc_read(int32_t fd, void* buf, int32_t nbytes);

/**rtc_write
* change the frequency, must be a multiple of 2
*/
int32_t rtc_write(int32_t fd, const void* buf, int32_t nbytes);



