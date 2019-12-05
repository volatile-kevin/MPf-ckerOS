#include "schedule.h"
#include "pcb.h"
#include "execute.h"
#include "halt.h"
#include "lib.h"
#include "rtc.h"
#include "keyboard.h"
#include "paging.h"

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
#define VIDEO_BUFFER1 (VIDEO + 0x1000)
#define VIDEO_BUFFER2 (VIDEO_BUFFER1 +  0x1000)
#define VIDEO_BUFFER3 (VIDEO_BUFFER2 +  0x1000)

volatile uint8_t pitCount = 0;
volatile uint8_t pitIntrCount = 0;

uint8_t cur_terminal;

/*
void init_tasks(){
    asm volatile(
    "movl %%eax, %%cr3\n\
     movl %0, %%eax"
    :"=m"(mainTask.regs.cr3)
    :
    :"%eax"
    );

    asm volatile(
    "pushfl \n\
     movl %%eax, (%%esp) \n\
     movl %0, %%eax \n\
     popfl"
    :"=m"(mainTask.regs.eflags)
    :
    :"%eax"
    );
}
*/

// void createTask(Task *task, uint32_t flags, uint32_t *pagedir){
//     // int i;
//     // int pid;
//     // for(i = 0; i < 6; i++){
//     //     if(PCB_array[i].state == 0){
//     //         pid = i;
//     //     }
//     // }
//     task->regs.eax = 0;
//     task->regs.ebx = 0;
//     task->regs.ecx = 0;
//     task->regs.edx = 0;
//     task->regs.esi = 0;
//     task->regs.edi = 0;
//     task->regs.eflags = flags;
//     task->regs.cr3 = (uint32_t) pagedir;
//     // task->regs.esp = PCB_array[pid].esp;
//     task->next = 0;

// }
// makes beep!
// take in frequency that you want the computer to play
// returns nothing
// side effects: Enables PIT interrupts
void beep(int frequency)
{
    uint32_t divisor = MAX_FREQ_PIT / frequency;
    outb(0xB6, PIT_CMD);
    outb((unsigned char) (divisor), PIT2);
    outb((unsigned char) (divisor >> BYTESHIFT), PIT2);

    unsigned char tempA = inb(SPEAKERPORT);

    if (tempA != (tempA | MASK)) 
    {
            outb(tempA | MASK, SPEAKERPORT);
    }

    enable_irq(PIT_IRQ); //tell the PIC to enable this interrupt

}

// initialize the PIT device
// takes in frequency of interrupts
// returns nothing
// side effects: enables PIT interrupts at 10 - 50ms 
void init_PIT(uint32_t frequency){

    uint32_t divisor = MAX_FREQ_PIT / frequency;

    outb(CMD_BYTE, PIT_CMD);

    uint8_t low = (uint8_t)(divisor & 0xFF);
    uint8_t high = (uint8_t)((divisor>>8) & 0xFF);

   // Send the frequency divisor.
    outb(low, PIT0);
    outb(high, PIT0);
    pitCount = 0;
    enable_irq(PIT_IRQ); //tell the PIC to enable this interrupt

}
// handles the PIT interrupts
// takes in nothing
// return nothing
// keeps a counter of amount of interrupts
// should spawn new shells at 0, 1, 2 interrupts
void pit_handler(){
    send_eoi(PIT_IRQ);
    //write to register C so that the interrupts can continously trigger
    // outb(REGC, PIT0);
    // // get rid of its data
    // inb(PIT_CMD);
    pitIntrCount++;
    //test_interrupts();
    //printf("fck\n");
    PCB_struct* pcb;
    
    if(pitCount < NUM_TERMINALS){
        switch_terminal(pitCount);
        pitCount++;
        execute((uint8_t *) "shell", 1);
    } else if (pitCount == NUM_TERMINALS) {
        switch_terminal(0);
        pitCount++;
    }

    //save the current tss state
    PCB_array[terminals[cur_terminal].curr_process].tss_state = tss;
    //update the scheduled terminal
    cur_terminal = (cur_terminal + 1)%3;
    //load the newly scheduled tss
    tss = PCB_array[terminals[cur_terminal].curr_process].tss_state;

    //page the video memory to write to
    if (cur_terminal == visible)
        map_video_page(0);
    else
        map_video_page(cur_terminal);

    switch_to_task();
}

/**
 * init_terminals
 * No Parameters
 * No Return
 * -----------------
 * Initializes all information needed by terminals.
 * Sets up the video buffers and clears all variables
 */
void init_terminals(){
    // setup initial video buffers for the terminal structs
    terminals[0].video_buffer = (uint8_t *)VIDEO_BUFFER1;
    memset((void*)VIDEO_BUFFER1, 0, FOUR_KB);
    terminals[1].video_buffer = (uint8_t *)VIDEO_BUFFER2;
    memset((void*)VIDEO_BUFFER2, 0, FOUR_KB);
    terminals[2].video_buffer = (uint8_t *)VIDEO_BUFFER3;
    memset((void*)VIDEO_BUFFER3, 0, FOUR_KB);

    //This for loop iterates all 3 terminal structs and sets all of the variables to 0/blank
    int i;
    for(i = 0; i < NUM_TERMINALS; i++){
        terminals[i].shell_pid = i;
        memset(terminals[i].video_buffer, 0, FOUR_KB);
        memset(terminals[i].buf_kb, 0, BUFFER_SIZE);
        terminals[i].screen_x = 0;
        terminals[i].screen_y = 0;
        terminals[i].curr_idx = 0;
        terminals[i].num_chars = 0;
        terminals[i].save_x = 0;
        terminals[i].save_y = 0;
        terminals[i].prev_num_chars = 0;
        terminals[i].curr_process = &PCB_array[i];
        memset(terminals[i].prev_buf, 0, BUFFER_SIZE);
    }
}

/**
 * switch_terminal
 * @param terminal_src
 * @param terminal_dest
 * Returns nothing
 * ----------------------------
 * Saves and restore info that is associated with each terminal and also updates which terminal is active
 */
void switch_terminal(uint8_t terminal_dest){

    // requested destination terminal index is the same as current (src) terminal
    if (terminal_dest == visible){
        return;
    }

    // supplied terminal indicies out of range
    if ((terminal_dest) > NUM_TERMINALS){
        return;
    }

    // stores keyboard information
    terminals[visible].screen_x = get_screen_x();
    terminals[visible].screen_y = get_screen_y();
    terminals[visible].curr_idx = get_curr_idx();
    terminals[visible].num_chars = get_num_chars();
    terminals[visible].save_x = get_save_x();
    terminals[visible].save_y = get_save_y();
    terminals[visible].prev_num_chars = get_previous_num_chars();

    // Saves the history buffer, keyboard buffer, and video buffer
    get_previous_buf(terminals[visible].prev_buf);
    memcpy(terminals[visible].video_buffer, (void*)VIDEO_MEM, FOUR_KB);
    memcpy(terminals[visible].buf_kb, buf_kb, BUFFER_SIZE);

    // Restores keyboard information
    set_cursor(terminals[terminal_dest].screen_x, terminals[terminal_dest].screen_y);
    set_curr_idx(terminals[terminal_dest].curr_idx);
    set_num_chars(terminals[terminal_dest].num_chars);
    set_save_x(terminals[terminal_dest].save_x);
    set_save_y(terminals[terminal_dest].save_y);
    set_previous_num_chars(terminals[terminal_dest].prev_num_chars);

    // Restore keyboard buff, history buff, and video buff
    memcpy((void*)VIDEO_MEM, terminals[terminal_dest].video_buffer, FOUR_KB);
    set_previous_buf(terminals[terminal_dest].prev_buf);
    memcpy(buf_kb, terminals[terminal_dest].buf_kb, BUFFER_SIZE);

    //Update current terminal
    visible = terminal_dest;
    return;
}

void switch_to_task(){
    
}

