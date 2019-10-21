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

