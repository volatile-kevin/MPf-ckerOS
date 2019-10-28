#ifndef KEYBOARD_H
#define KEYBOARD_H

#define STATUS_REGISTER 0x64
#define MAXCHAR 127
#define BUFFER_SIZE 128
// written output buffer
// The last byte is for 0 which is why there is a + 1
char buf_kb[BUFFER_SIZE];


// initialize keyboard for input
void init_keyboard();

// handle input and output for keyboard
void keyboard_handler();

#endif // keyboard.h
