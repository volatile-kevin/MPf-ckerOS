#include "schedule.h"
#include "pcb.h"
#include "execute.h"
#include "halt.h"
#include "lib.h"
#include "rtc.h"
#include "keyboard.h"
#include "paging.h"
#include "x86_desc.h"



volatile uint8_t pitCount = 0;

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
    pitIntrCount = 0;
    enable_irq(PIT_IRQ); //tell the PIC to enable this interrupt

}
// handles the PIT interrupts
// takes in nothing
// return nothing
// keeps a counter of amount of interrupts
// should spawn new shells at 0, 1, 2 interrupts
void pit_handler(){
    //map_video_page(0);
    //printf("cur term = %d\n", cur_terminal);
    //test_interrupts();
    uint32_t esp;
    uint32_t ebp;
    asm volatile ( //saving parent esp and ebp
        "movl %%esp, %0 \n\
         movl %%ebp, %1"
        :"=r"(esp), "=r"(ebp) //outputs
        : //input operands
        :"memory" //clobbers
    );
    pitIntrCount++;
    //save the current tss state
    terminals[cur_terminal].save_tss = tss;
    PCB_array[terminals[cur_terminal].curr_process].task_esp = esp;
    PCB_array[terminals[cur_terminal].curr_process].task_ebp = ebp;

    //update the scheduled terminal
    cur_terminal = (cur_terminal + 1)%NUM_TERMINALS;

    if(pitCount < NUM_TERMINALS){
        switch_terminal(pitCount);
        pitCount++;
        //send_eoi(PIT_IRQ);
        execute((uint8_t *) "shell");
    } else if (pitCount == NUM_TERMINALS) {
        switch_terminal(0);
        pitCount++;
    }

    //load the newly scheduled tss
    tss = terminals[cur_terminal].save_tss;
    esp = PCB_array[terminals[cur_terminal].curr_process].task_esp;
    ebp = PCB_array[terminals[cur_terminal].curr_process].task_ebp;

    //page the video memory to write to
    if (cur_terminal == visible)
        map_video_page(0);
    else
        map_video_page(1+cur_terminal); //map into a video buffer (not main)

    map_page((void*)((terminals[cur_terminal].curr_process + PIDOFFSET)*FOUR_MB), (void*)VADDRPROGPAGE, USWFLAGS);
    asm volatile(
        "movl %0, %%esp\n\
        movl %1, %%ebp"

        : //output operands
        : "r"(esp), "r"(ebp)//input operands
        : "memory"
        );
    send_eoi(PIT_IRQ);
    return;
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
    memset((void*)VIDEO_BUFFER2, 20, FOUR_KB);
    terminals[2].video_buffer = (uint8_t *)VIDEO_BUFFER3;
    memset((void*)VIDEO_BUFFER3, 40, FOUR_KB);

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
        terminals[i].curr_process = -1;
        terminals[i].enter_flag = 1;
        memset(terminals[i].prev_buf, 0, BUFFER_SIZE);
    }
    cur_terminal = 0;
    visible = 0;
    pitIntrCount = 0;
}

/**
 * switch_terminal
 * @param terminal_dest
 * Returns nothing
 * ----------------------------
 * Saves and restore info that is associated with each terminal and also updates which terminal is active
 */
void switch_terminal(uint8_t terminal_dest){
    // requested destination terminal index is the same as current (src) terminal
//    if (pitCount > 3){
//     sti();
//     while (cur_terminal != visible);
//     cli();
//    }

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

