#include "multiboot.h"
#include "x86_desc.h"
#include "lib.h"
#include "i8259.h"
#include "debug.h"
#include "tests.h"
#include "keyboard.h"

#define NUM_CHARACTERS 0x3B
#define KEYBOARD_PORT 0x60

// array of characters, indicies map to scan codes on keyboard
char char_array[NUM_CHARACTERS] = {'!', '!', '1', '2', '3', '4', '5', '6', '7', '8', '9', '0', '-', '=', '!', '!',
	 'q', 'w', 'e', 'r', 't', 'y', 'u', 'i', 'o', 'p', '[', ']', '!', '!', 'a', 's',
	 'd', 'f', 'g', 'h', 'j', 'k', 'l' , ';', '\'', '`', '!', '\\', 'z', 'x', 'c', 'v',
	 'b', 'n', 'm',',', '.', '/', '!', '*', '!', ' ', '!'};


/* 
 * init_keyboard
 *   DESCRIPTION: Initializes the keyboard for use.
 *   INPUTS: none
 *   OUTPUTS: none
 *   RETURN VALUE: none
 *   SIDE EFFECTS: keyboard is enabled on PIC
 */
void init_keyboard(){
    // enable keyboard interrupt on the PIC
    enable_irq(1);
}

/* 
 * keyboard_handler
 *   DESCRIPTION: Takes input from keyboard and displays it
 *   INPUTS: none (implicit inb() from the keyboard port)
 *   OUTPUTS: none
 *   RETURN VALUE: none
 *   SIDE EFFECTS: keyboard typed value is read and put on screen
 */
void keyboard_handler(){
    // clear interrupts
    cli();

    // initialize vars
    unsigned int val;
    char output_to_display;

    // get character from keyboard port
    val = inb(KEYBOARD_PORT);

    // if we are within our possible characters
    if ((val > 0) & (val < NUM_CHARACTERS)){

        output_to_display = char_array[val];
        // if the character is valid, print it
        if (output_to_display != '!'){
            putc(output_to_display);
        }
    }
    // restore interrupts
    sti();
    // send end of interrupt signal
    send_eoi(1);
 }

