#include "pcb.h"
#include "execute.h"
#include "halt.h"
#include "lib.h"
#include "types.h"
#include "rtc.h"

void beep(int frequency);

extern void init_PIT(uint32_t frequency);
extern void pit_handler();
void spawn_shells(int pid);


uint8_t visible;

typedef struct terminal_t {
    int shell_pid; //current shell that corresponds to this terminal
    uint8_t* video_buffer; //pointer to this terminals video buffer
    char buf_kb[128]; //keyboard buffer
} terminal_t;


terminal_t terminals[3];

