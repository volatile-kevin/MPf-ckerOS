#include "multiboot.h"
#include "x86_desc.h"
#include "lib.h"
#include "i8259.h"
#include "debug.h"
#include "tests.h"
#include "keyboard.h"


/*
############### Ports:
IO Port	Access Type	Purpose
0x60	Read/Write	Data Port
0x64	Read	Status Register
0x64	Write	Command Register

Data Port:
The Data Port (IO Port 0x60) is used for reading data that was received from a 
PS/2 device or from the PS/2 controller itself and writing data to a PS/2 device 
or to the PS/2 controller itself.

############### Status Register:

Bit	                    Meaning
0	Output buffer status (0 = empty, 1 = full)
(must be set before attempting to read data from IO port 0x60)

1	Input buffer status (0 = empty, 1 = full)
(must be clear before attempting to write data to IO port 0x60 or IO port 0x64)

2	System Flag
Meant to be cleared on reset and set by firmware (via. PS/2 Controller Configuration Byte) if the system passes self tests (POST)

3	Command/data (0 = data written to input buffer is data for PS/2 device, 1 = data written to input buffer is data for PS/2 controller command)
4	Unknown (chipset specific)
May be "keyboard lock" (more likely unused on modern systems)

5	Unknown (chipset specific)
May be "receive time-out" or "second PS/2 port output buffer full"

6	Time-out error (0 = no error, 1 = time-out error)
7	Parity error (0 = no error, 1 = parity error)

Command Register:
The Command Port (IO Port 0x64) 
is used for sending commands to the PS/2 Controller (not to PS/2 devices).

*/

void init_keyboard(){
    // enable keyboard interrupt
    enable_irq(1);
}


void keyboard_handler(unsigned int kb_data){




 }

