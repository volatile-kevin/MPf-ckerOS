#include "multiboot.h"
#include "x86_desc.h"
#include "lib.h"
#include "i8259.h"
#include "debug.h"
#include "tests.h"
#include "keyboard.h"
#include "types.h"
#include "terminal.h"

#define NUM_CHARACTERS 0x3B
#define KEYBOARD_PORT 0x60

//IMPORTANT SCANCODES
#define BACKSPACE 0x0E
#define ENTER 0x1C
#define CTRL_DOWN 0x1D
#define CTRL_UP 0x9D
#define LSHIFT_DOWN 0x2A
#define LSHIFT_UP 0xAA
#define RSHIFT_DOWN 0x36
#define RSHIFT_UP 0xB6
#define CAPS_DOWN 0x3A
#define L_SCANCODE 0x26
#define UP_DOWN 0X48

#define NUMFLAGS 4
#define ENABLED 1
#define DISABLED 0

#define LSHIFTIDX 0
#define RSHIFTIDX 1
#define CTRLIDX 2
#define CAPSIDX 3
#define IRQ_KB 1

int num_chars = 0;
int curr_idx = 0;
int save_x, save_y;
char previous_buf[BUFFER_SIZE];
int previous_num_chars;

// array of characters, indicies map to scan codes on keyboard
char char_array[NUM_CHARACTERS] = {0, 0, '1', '2', '3', '4', '5', '6', '7', '8', '9', '0', '-', '=', 0, 0,
	 'q', 'w', 'e', 'r', 't', 'y', 'u', 'i', 'o', 'p', '[', ']', 0, 0, 'a', 's',
	 'd', 'f', 'g', 'h', 'j', 'k', 'l' , ';', '\'', '`', 0, '\\', 'z', 'x', 'c', 'v',
	 'b', 'n', 'm',',', '.', '/', 0, '*', 0, ' ', 0};

char char_array_shift_nocap[NUM_CHARACTERS] = {0, 0, '!', '@', '#', '$', '%', '^', '&', '*', '(', ')', '_', '+', 0, 0,
	 'Q', 'W', 'E', 'R', 'T', 'Y', 'U', 'I', 'O', 'P', '{', '}', 0, 0, 'A', 'S',
	 'D', 'F', 'G', 'H', 'J', 'K', 'L' , ':', '\"', '~', 0, '|', 'Z', 'X', 'C', 'V',
	 'B', 'N', 'M','<', '>', '?', 0, '*', 0, ' ', 0};

char char_array_shift_cap[NUM_CHARACTERS] = {0, 0, '!', '@', '#', '$', '%', '^', '&', '*', '(', ')', '_', '+', 0, 0,
	 'q', 'w', 'e', 'r', 't', 'y', 'u', 'i', 'o', 'p', '{', '}', 0, 0, 'a', 's',
	 'd', 'f', 'g', 'h', 'j', 'k', 'l' , ':', '\"', '~', 0, '|', 'z', 'x', 'c', 'v',
	 'b', 'n', 'm','<', '>', '?', 0, '*', 0, ' ', 0};

char char_array_noshift_cap[NUM_CHARACTERS] = {0, 0, '1', '2', '3', '4', '5', '6', '7', '8', '9', '0', '-', '=', 0, 0,
	 'Q', 'W', 'E', 'R', 'T', 'Y', 'U', 'I', 'O', 'P', '[', ']', 0, 0, 'A', 'S',
	 'D', 'F', 'G', 'H', 'J', 'K', 'L' , ';', '\'', '`', 0, '\\', 'Z', 'X', 'C', 'V',
	 'B', 'N', 'M',',', '.', '/', 0, '*', 0, ' ', 0};

// l-shift, r-shift, ctrl, caps-lock
int flag_arr[NUMFLAGS] = {0, 0, 0, 0};

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
    enable_irq(IRQ_KB); //1 is the keyboard port on the pic
    buf_kb[MAXCHAR] = 0; //Set the last character in the buffer to 0
		enter_flag = 1;
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

// terminal write a backspace, putc
    // initialize vars
    unsigned int scanCode;
    char output_to_display;
    // get character from keyboard port
    scanCode = inb(KEYBOARD_PORT);

// l-shift, r-shift, ctrl, caps-lock, enter

    // check if clear was sent
    switch(scanCode){
        case LSHIFT_DOWN: // left-shift pressed
            flag_arr[LSHIFTIDX] = ENABLED;
            send_eoi(IRQ_KB);
            return;
        case LSHIFT_UP: // left-shift released
            flag_arr[LSHIFTIDX] = DISABLED;
            send_eoi(IRQ_KB);
            return;
        case RSHIFT_DOWN: // right-shift pressed
            flag_arr[RSHIFTIDX] = ENABLED;
            send_eoi(IRQ_KB);
            return;
        case RSHIFT_UP: // right-shift released
            flag_arr[RSHIFTIDX] = DISABLED;
            send_eoi(IRQ_KB);
            return;
        case CTRL_DOWN: // control pressed
            flag_arr[CTRLIDX] ++;
            send_eoi(IRQ_KB);
            return;
        case CTRL_UP: // control released
            flag_arr[CTRLIDX] --;
            send_eoi(IRQ_KB);
            return;
        case CAPS_DOWN: // caps-lock pressed
            if(flag_arr[CAPSIDX])
                flag_arr[CAPSIDX] = DISABLED;
            else
                flag_arr[CAPSIDX] = ENABLED;
            // sti();
            send_eoi(IRQ_KB);
            return;
        case UP_DOWN:
            while(get_screen_x() != save_x && get_screen_y() != save_y){
                backspace();
            }
            memmove(&buf_kb, &previous_buf, BUFFER_SIZE);
            num_chars = previous_num_chars;
            curr_idx = num_chars;
            printf(buf_kb);
            send_eoi(IRQ_KB);
            return;
        default:
            break;
    }

    // clear the screen
    // CLEARING THE SCREEN ALSO CLEARS THE BUFFER DONT @ ME
    if (flag_arr[CTRLIDX] && scanCode == L_SCANCODE){
        // if L is now being pressed
        clear_kb();
        curr_idx = 0;
        num_chars = 0;
        memset(&buf_kb, 0, BUFFER_SIZE);
    }

    // Handles backspace.
    // We only want to backspace if there is something in our buffer
    // that is to the left of our current index
    else if(scanCode == BACKSPACE && curr_idx){
        backspace();
        curr_idx--;
        num_chars--;
    }

    // Handles enter.
    // We want a clean buffer and a new line each time we hit enter
    else if (scanCode == ENTER){

        memmove(previous_buf, buf_kb, BUFFER_SIZE);
        previous_num_chars = num_chars;
        buf_kb[curr_idx] = '\n';
        // ******************** Call terminal read *********************
        putc('\n');
        enter_flag = 0;
        curr_idx = 0;
        num_chars = 0;
    }

// l-shift, r-shift, l-ctrl, r-ctrl, caps-lock, enter
    // if we are within our possible characters
    else if (num_chars < MAXCHAR && ((scanCode > 0) & (scanCode < NUM_CHARACTERS))){
        if(strlen(buf_kb) == 0){
            save_x = get_screen_x();
            save_y = get_screen_y();
        }
        // if one or both shifts are on
        if (flag_arr[LSHIFTIDX] || flag_arr[RSHIFTIDX]){
            // if caps lock is on also
            if (flag_arr[CAPSIDX]){
                // lowercase
                output_to_display = char_array_shift_cap[scanCode];
                if (output_to_display != 0){
                    // add that character to the buffer
                    putc(output_to_display);
                    buf_kb[curr_idx] = output_to_display;
                    curr_idx++;
                    num_chars++;
                }
            }
            else{
                output_to_display = char_array[scanCode];
                //uppercase
                output_to_display = char_array_shift_nocap[scanCode];
                if (output_to_display != 0){
                    // add that character to the buffer
                    putc(output_to_display);
                    buf_kb[curr_idx] = output_to_display;
                    curr_idx++;
                    num_chars++;
                }
            }
        }
        // if caps is on but not shift
        else if (flag_arr[CAPSIDX]){
            //uppercase
            output_to_display = char_array_noshift_cap[scanCode];
            if (output_to_display != 0){
                // add that character to the buffer
                putc(output_to_display);
                buf_kb[curr_idx] = output_to_display;
                curr_idx++;
                num_chars++;
            }
        }
        // if neither caps lock nor shift is on
        else{
            //lowercase
            output_to_display = char_array[scanCode];
            // if the character is valid, print it
            if (output_to_display != 0){
                // add that character to the buffer
                putc(output_to_display);
                buf_kb[curr_idx] = output_to_display;
                curr_idx++;
                num_chars++;
            }
        }
    } else {
        // A scancode that we do not currently support, its bad and should feel bad
    }
    // restore interrupts
    // sti();
    // send end of interrupt signal
    send_eoi(IRQ_KB);
 }
