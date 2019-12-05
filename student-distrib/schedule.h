#include "pcb.h"
#include "x86_desc.h"
#include "schedule_asm.h"

	
	
#define BUFFER_SIZE 128
#define NUM_TERMINALS 3

void beep(int frequency);


uint8_t visible;

typedef struct terminal_t {
    int shell_pid; //current shell that corresponds to this terminal
    uint8_t* video_buffer; //pointer to this terminals video buffer
    tss_t tss_save;
    char buf_kb[BUFFER_SIZE]; //keyboard buffer
    uint8_t screen_x; // screen logical location x
    uint8_t screen_y; // screen logical location y
    uint8_t curr_idx; // current location in command
    uint8_t num_chars;
    uint8_t save_x;
    uint8_t save_y;
    uint8_t prev_num_chars;
    char prev_buf[BUFFER_SIZE];
    uint8_t curr_process;
} terminal_t;


// typedef struct {
//     uint32_t eax, ebx, ecx, edx, esi, edi, esp, ebp, eip, eflags, cr3;
// } Registers;
 
// typedef struct Task {
//     Registers regs;
//     struct Task *next;
// } Task;


// static Task * runningTask;
// static Task mainTask;
// static Task otherTask;
terminal_t terminals[3];

extern void init_terminals();
extern void init_PIT(uint32_t frequency);
extern void pit_handler();
extern void switch_terminal(uint8_t terminal_dest);


// void init_tasks();
// void createTask(Task *task, uint32_t flags, uint32_t *pagedir);
