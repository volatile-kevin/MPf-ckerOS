#include "multiboot.h"
#include "x86_desc.h"
#include "lib.h"
#include "i8259.h"
#include "debug.h"
#include "tests.h"
#include "keyboard.h"
#include "terminal.h"

/*
 * terminal_read
 *   DESCRIPTION: reads FROM the keyboard buffer into buf, return number of bytes read
 *   INPUTS: fd - file descriptor
 *           buf - buffer to read
 *           nbytes - num bytes to read
 *   OUTPUTS: num of bytes read
 *   RETURN VALUE: num of bytes read
 *   SIDE EFFECTS: buf is read
 */
int32_t terminal_read (int32_t fd, void* buf, int32_t nbytes){
    while(enter_flag){}
    // if buffer size less than nbytes set max to that, otherwise set to nbytes
    int count = 0;
    for (count = 0; buf_kb[count]!='\n'; count++){}
    count++;
    int max = BUFFER_SIZE < nbytes ? BUFFER_SIZE : nbytes;
    max = max < count ? max : count;
    // move that data into internal buffer from our kb buffer
    memmove(buf, &buf_kb,max);
    enter_flag = 1;
    return max;
}

/*
 * terminal_write
 *   DESCRIPTION: writes TO the screen from buf, return number of bytes written or -1
 *   INPUTS: fd - file descriptor
 *           buf - buffer to read
 *           nbytes - num bytes to read
 *   OUTPUTS: num of bytes written
 *   RETURN VALUE: num of bytes written
 *   SIDE EFFECTS: buf is read and written to screen
 */
int32_t terminal_write (int32_t fd, const void* buf, int32_t nbytes){
    int i;
    int count = 0;

    // putc up to nbytes in the buf
    for(i = 0; i < nbytes; i++){
        putc((*(uint8_t *)((uint32_t)buf + i)));
        count = count + 1;
    }
    return count;
}

/*
 * terminal_open
 *   DESCRIPTION: initializes terminal stuff (or nothing), return 0
 *   INPUTS: filename - name of terminal file to open
 *   OUTPUTS: zero
 *   RETURN VALUE: zero
 *   SIDE EFFECTS: terminal is ready
 */
int32_t terminal_open (const uint8_t* filename){
    return 0;
}

/*
 * terminal_close
 *   DESCRIPTION: clears any terminal specific variables (or do nothing), return 0
 *   INPUTS: fd - file descriptor
 *   OUTPUTS: zero
 *   RETURN VALUE: zero
 *   SIDE EFFECTS: terminal is cleared
 */
int32_t terminal_close (int32_t fd){
    return 0;
}
