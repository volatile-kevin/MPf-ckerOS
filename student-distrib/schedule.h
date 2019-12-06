#include "pcb.h"
#include "x86_desc.h"

#define BUFFER_SIZE 128
#define NUM_TERMINALS 3
#define STARTING_PIT_FREQUENCY 20
void beep(int frequency);

uint8_t visible;
uint8_t cur_terminal;
volatile uint8_t pitIntrCount;

typedef struct terminal_t {
    int shell_pid; //current shell that corresponds to this terminal
    uint8_t* video_buffer; //pointer to this terminals video buffer
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
} terminal_t;

terminal_t terminals[NUM_TERMINALS];

void init_terminals();
void init_PIT(uint32_t frequency);
void pit_handler();
void switch_terminal(uint8_t terminal_dest);
