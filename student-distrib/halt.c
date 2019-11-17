#include "filesys.h"
#include "multiboot.h"
#include "lib.h"
#include "tests.h"
#include "types.h"
#include "pcb.h"
#include "x86_desc.h"
#include "paging.h"
#include "execute.h"

#define NUMPROCESS 6
#define PIDOFF 2
#define VADDR 0x08000000
#define PFLAGS 0x87

// halt: halts the current process
// parameter: executes ret val
// returns 0 on success
// side effects: will deactivate the current process and activate its parent
// halts by making all PCB members garbage = -1
int32_t halt(uint8_t status){


    // get the active pcb
    int i;
    for(i = 0; i < NUMPROCESS; i++){
        if(PCB_array[i].state == 0)
            break;
    }
    int curr_pcb = i;
    if(curr_pcb == 0){
      // return -1;
      // PCB_array[curr_pcb].state = -1;
      // PCB_array[curr_pcb].pcb_in_use = -1;
      while(1){
        execute("shell");
      }
    }
    // destruct FDT
    for (i = 0; i < NUMPROCESS; i++){
        PCB_array[curr_pcb].fd_table[i].file_pos = -1;
        PCB_array[curr_pcb].fd_table[i].fileType = -1;
        PCB_array[curr_pcb].fd_table[i].flags_arr[0] = -1;
        PCB_array[curr_pcb].fd_table[i].flags_arr[1] = -1;
        PCB_array[curr_pcb].fd_table[i].flags_arr[2] = -1;
        PCB_array[curr_pcb].fd_table[i].flags_arr[3] = -1;
        PCB_array[curr_pcb].fd_table[i].flags_arr[4] = -1;

        PCB_array[curr_pcb].fd_table[i].inode_number = -1;
        PCB_array[curr_pcb].fd_table[i].jump_start_idx = 0;
    }


    // destruct PCB
    PCB_array[curr_pcb].process_id = -1;
    PCB_array[curr_pcb].pcb_in_use = -1;
    PCB_array[curr_pcb].state = -1;

    int esp = PCB_array[curr_pcb].esp;
    PCB_array[curr_pcb].esp = -1;

    int ebp = PCB_array[curr_pcb].ebp;
    PCB_array[curr_pcb].ebp = -1;

    int return_label_address = PCB_array[curr_pcb].return_label_add;

    PCB_array[curr_pcb].return_label_add = -1;

    int parent_pid = PCB_array[curr_pcb].parent_pid;
     PCB_array[parent_pid].state = 0;
     PCB_array[curr_pcb].parent_pid = -1;



    // update the tss
    tss.ss0 = PCB_array[curr_pcb].ss0;
    tss.esp0 = PCB_array[curr_pcb].esp0;

    // allocate page
    map_page((void*)((parent_pid + PIDOFF)*FOUR_MB), (void*)VADDR, PFLAGS);

    // jump to the addr of the label basically
    asm volatile(
            "movl %0, %%esp\n\
             movl %1, %%ebp \n\
             jmp *%2"
            : //input operands
            : "r"(esp), "r"(ebp), "r"(return_label_address)//clobbers and goto labels
            );
    return 0;
}
