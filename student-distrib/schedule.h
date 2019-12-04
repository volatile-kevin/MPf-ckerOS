#include "pcb.h"
#include "execute.h"
#include "halt.h"
#include "lib.h"
#include "types.h"
#include "rtc.h"
#include "x86_desc.h"
#include "schedule_asm.h"

void beep(int frequency);


uint8_t visible;

typedef struct terminal_t {
    int shell_pid; //current shell that corresponds to this terminal
    uint8_t* video_buffer; //pointer to this terminals video buffer
    char buf_kb[128]; //keyboard buffer
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

extern void init_PIT(uint32_t frequency);
extern void pit_handler();
void spawn_shells(int pid);
void switch_terminal(uint8_t terminal);
// void init_tasks();
// void createTask(Task *task, uint32_t flags, uint32_t *pagedir);
