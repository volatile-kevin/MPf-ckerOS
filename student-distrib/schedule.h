#include "pcb.h"
#include "x86_desc.h"

#define BUFFER_SIZE 128
#define NUM_TERMINALS 3
#define STARTING_PIT_FREQUENCY 20

#define PIT0 0x40         
#define PIT1 0x41
#define PIT2 0x42
#define PIT_CMD 0x43
#define MAX_FREQ_PIT 1193180
#define CMD_BYTE 0x36
#define PIT_IRQ 0
#define BYTESHIFT 8
#define SPEAKERPORT 0x61
#define MASK 3

#define VIDEO       0xB8000
#define NUM_COLS    80
#define NUM_ROWS    25
#define VIDEO_BUFFER1 (VIDEO + FOUR_KB)
#define VIDEO_BUFFER2 (VIDEO_BUFFER1 +  FOUR_KB)
#define VIDEO_BUFFER3 (VIDEO_BUFFER2 +  FOUR_KB)

// makes beep!
// take in frequency that you want the computer to play
// returns nothing
// side effects: Enables PIT interrupts
void beep(int frequency);

uint8_t visible;
uint8_t cur_terminal;
volatile uint8_t pitIntrCount;
typedef struct terminal_t {
    int shell_pid; //current shell that corresponds to this terminal
    uint8_t* video_buffer; //pointer to this terminals video buffer
    uint8_t** screen_start;
    char buf_kb[BUFFER_SIZE]; //keyboard buffer
    tss_t save_tss;
    uint8_t screen_x; // screen logical location x
    uint8_t screen_y; // screen logical location y
    uint8_t curr_idx; // current location in command
    uint8_t num_chars;
    uint8_t save_x;
    uint8_t save_y;
    uint8_t prev_num_chars;
    char prev_buf[BUFFER_SIZE];
    int curr_process;
    char enter_flag;
} terminal_t;

terminal_t terminals[NUM_TERMINALS];

/**
 * init_terminals
 * No Parameters
 * No Return
 * -----------------
 * Initializes all information needed by terminals.
 * Sets up the video buffers and clears all variables
 */
void init_terminals();


// initialize the PIT device
// takes in frequency of interrupts
// returns nothing
// side effects: enables PIT interrupts at 10 - 50ms 
void init_PIT(uint32_t frequency);


// handles the PIT interrupts
// takes in nothing
// return nothing
// keeps a counter of amount of interrupts
// should spawn new shells at 0, 1, 2 interrupts
void pit_handler();

/**
 * switch_terminal
 * @param terminal_dest
 * Returns nothing
 * ----------------------------
 * Saves and restore info that is associated with each terminal and also updates which terminal is active
 */
void switch_terminal(uint8_t terminal_dest);
