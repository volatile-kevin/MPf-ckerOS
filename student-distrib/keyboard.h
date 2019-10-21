#ifndef KEYBOARD_H
#define KEYBOARD_H

#define STATUS_REGISTER 0x64

// initialize keyboard for input
void init_keyboard();

// handle input and output for keyboard
void keyboard_handler();

#endif // keyboard.h
