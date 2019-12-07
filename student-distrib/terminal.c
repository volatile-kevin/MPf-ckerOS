#include "multiboot.h"
#include "x86_desc.h"
#include "lib.h"
#include "i8259.h"
#include "debug.h"
#include "tests.h"
#include "keyboard.h"
#include "terminal.h"
#include "schedule.h"
#include "types.h"

/**
 * terminal_read
 *   DESCRIPTION: reads FROM the keyboard buffer into buf, return number of bytes read
 *   INPUTS: fd - file descriptor
 *           buf - buffer to read
 *           nbytes - num bytes to read
 *   OUTPUTS: num of bytes read
 *   RETURN VALUE: num of bytes read
 *   SIDE EFFECTS: buf is read
 */
int32_t terminal_read(int32_t fd, void *buf, int32_t nbytes) {

    while (terminals[visible].enter_flag) {
    }
    while (cur_terminal != visible);
    cli();

    memset(storeargs, 0, ARG_LENGTH);
    // if buffer size less than nbytes set max to that, otherwise set to nbytes
    int count = 0;

    int spacecount = -1; //counts the number of spaces in input
    int offset_arg = 0;  //keeps track of how many bytes we have currently written to the argument

    int max = BUFFER_SIZE < nbytes ? BUFFER_SIZE : nbytes;
    //copy in the args
    for (count = 0; buf_kb[count] != '\n'; count++) {
        if (spacecount > -1) {
            storeargs[offset_arg] = buf_kb[count]; //set the current byte into the arg buffer
            offset_arg++;
        }

        if (buf_kb[count] == ' ' && spacecount == -1) {
            spacecount++; //increment which buffer we write to
            offset_arg = 0; //reset offset_arg bc we will be writing to new arg buffer
            max = count;
        }

    }
    count++;
    max = max < count ? max : count;
    max = max < nbytes ? max : nbytes;
    // move that data into internal buffer from our kb buffer
    memmove(buf, &buf_kb, max);
    terminals[visible].enter_flag = 1;

    sti();
    return max;
}

/**
 * terminal_write
 *   DESCRIPTION: writes TO the screen from buf, return number of bytes written or -1
 *   INPUTS: fd - file descriptor
 *           buf - buffer to read
 *           nbytes - num bytes to read
 *   OUTPUTS: num of bytes written
 *   RETURN VALUE: num of bytes written
 *   SIDE EFFECTS: buf is read and written to screen
 */
int32_t terminal_write(int32_t fd, const void *buf, int32_t nbytes) {
    int i;
    int count = 0;

    // putc up to nbytes in the buf
    for (i = 0; i < nbytes; i++) {
        putc((*(uint8_t *) ((uint32_t) buf + i)));
        count = count + 1;
    }
    return count;
}

/**
 * terminal_open
 *   DESCRIPTION: initializes terminal stuff (or nothing), return 0
 *   INPUTS: filename - name of terminal file to open
 *   OUTPUTS: zero
 *   RETURN VALUE: zero
 *   SIDE EFFECTS: terminal is ready
 */
int32_t terminal_open(const uint8_t *filename) {
    return 0;
}

/**
 * terminal_close
 *   DESCRIPTION: clears any terminal specific variables (or do nothing), return 0
 *   INPUTS: fd - file descriptor
 *   OUTPUTS: zero
 *   RETURN VALUE: zero
 *   SIDE EFFECTS: terminal is cleared
 */
int32_t terminal_close(int32_t fd) {
    return 0;
}


/**terminal_getargs
 * copy over the arguments from the last terminal read into a buffer
 * @param0 = destination pointer to char buffer
 * @param1 = number of bytes to be copied
 * @returns = nothing, you get nothing. And you can either like it or not
 * but I really don't give a care. Not one
 */
void terminal_getargs(uint8_t *dest, int32_t nbytes) {
    uint32_t i = 0;
    for (; i < nbytes; i++) {
        dest[i] = storeargs[i];
    }
    dest[nbytes - 1] = '\n'; //just in case
    return;
}
