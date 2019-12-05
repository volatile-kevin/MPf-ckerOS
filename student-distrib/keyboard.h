#ifndef KEYBOARD_H
#define KEYBOARD_H

#define STATUS_REGISTER 0x64
#define MAXCHAR 127
#define BUFFER_SIZE 128
// written output buffer
// The last byte is for 0 which is why there is a + 1
char buf_kb[BUFFER_SIZE];
char enter_flag;
int get_curr_idx();
int get_num_chars();
int get_save_x();
int get_save_y();
void get_previous_buf(char* target_buf);
int get_previous_num_chars();
//Setters used in multi terminal
void set_curr_idx(int x);
void set_num_chars(int x);
void set_save_x(int x);
void set_save_y(int x);
void set_previous_buf(char* source_buf);
void set_previous_num_chars(int x);

// initialize keyboard for input
void init_keyboard();

// handle input and output for keyboard
void keyboard_handler();

#endif // keyboard.h
