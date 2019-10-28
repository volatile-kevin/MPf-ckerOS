#ifndef TERMINAL_H
#define TERMINAL_H

#define STATUS_REGISTER 0x64

// reads FROM the keyboard buffer into buf, return number of bytes read
extern int32_t terminal_read (int32_t fd, void* buf, int32_t nbytes);

// writes TO the screen from buf, return number of bytes written or -1
extern int32_t terminal_write (int32_t fd, const void* buf, int32_t nbytes);

// initializes terminal stuff (or nothing), return 0
extern int32_t terminal_open (const uint8_t* filename);

// clears any terminal specific variables (or do nothing), return 0
extern int32_t terminal_close (int32_t fd);


#endif // keyboard.h
