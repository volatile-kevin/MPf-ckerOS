#include "execute.h"
#include "x86_desc.h"
#include "execute_asm.h"
#include "types.h"
#include "filesys.h"
#include "pcb.h"
#include "paging.h"
#include "lib.h"
#include "schedule.h"





uint32_t firstInstruct; //entrypoint of program img
PCB_struct PCB_array[NUMPROCESSES]; //array of PCBs, maximum processes for this cp is 6

// wrapper function that does essentially nothing, but apparently we need it or else everything fucking breaks
// parameters: entrypoint address
// returns nothing
// side effect: will jump to user
void switch_to_user_mode(uint32_t entry){
  // call the asm function
  switch_to_usr(entry);
}

// copies the header into a buffer, which is the parameter
// will be used to check the ELF magic number and get firstInstruct
// parameters: filename, ptr to buffer, file descriptor
// returns: bytes copied
// side effect: filesys
int read_ELF(const uint8_t* fname, uint8_t* buf, int32_t fd){
  int i;
  int nbytes = HEADERSIZE;
  i = file_open(fname);
  if(i == -1){
    return -1;
  }
  i = file_read(fd, buf, nbytes);
  return i;
}

// loads the entire program image to memory
// uses a higher bound MAXBYTES, the space allocated for the program image
// parameters: filename, ptr to buffer, file descriptor
// returns: bytes copied
// side effect: filesys uses a higherbound of size of file for nbytes, MAXBYTES will likely never be hit
int load(const uint8_t* fname, uint8_t* buf){
  int i;
  int nbytes = MAXBYTES;
  i = file_open(fname);
  i = file_load(buf, nbytes);
  return i;
}

// Executes the file, will use a context switch
// reads the ELF, gets entrypoint, error checks, flushes TLB
// parameters: filename
// returns: -1 on failure, 0 on success
// side effect: will go to userspace through context switch. Child processes will return to label before return
int execute(const uint8_t* fname, uint8_t switch_after_creation){
  // copy into this buffer
  uint8_t switch_after_creation_temp = switch_after_creation;
  uint8_t temp[MAXCOMMANDSIZE];
  memmove(temp, fname, MAXCOMMANDSIZE);
  uint8_t buf[HEADERBUFSIZE];

  int fd_avail = -1;
  int error = read_ELF(fname, buf, fd_avail);
  if(error == -1){
    printf("invalid command\n");
    return -1;
  }
  // get the first available PID
  int pid = getPID();
  //error check
  if(pid == -1){
    printf("no more available processes\n");
    return -1;
  }


  int esp;
  int ebp;
  int return_label_add;

    asm volatile ( //saving parent esp and ebp
          "movl %%esp, %0 \n\
           movl %%ebp, %1 \n\
           leal ret_label, %%eax \n\
           movl %%eax, %2"
           :"=r"(esp), "=r"(ebp), "=m"(return_label_add) //output operands
           : //input operands
           :"%eax","memory" //clobbers
          );
  // fill PCB members
  PCB_array[pid].esp = esp;
  PCB_array[pid].ebp = ebp;
  PCB_array[pid].return_label_add = return_label_add;
  PCB_array[pid].esp0 = tss.esp0;
  PCB_array[pid].ss0 = tss.ss0;
  PCB_array[pid].parent_pid = -1;
  int x = 0;
  // find the active process
  // for (; x < NUMPROCESSES; x++){
  //   if (PCB_array[x].state == 0){
  //     PCB_array[pid].parent_pid = x;
  //     PCB_array[x].state = -1;
  //     break;
  //   }
  // }
  // set active

  //set parent pid if the terminal has an active process
  if (terminals[cur_terminal].curr_process != -1){//if this terminal has an active process
    //set the new process parent pid to active process
    PCB_array[pid].parent_pid = terminals[cur_terminal].curr_process;
    //set active process state to inactive
    PCB_array[terminals[cur_terminal].curr_process].state = -1;
    //set current terminal curr_process to newly created process
    terminals[cur_terminal].curr_process = pid;
  }

  //set this process to active
  PCB_array[pid].state = 0;


  // allocate a page
  map_page((void*)((pid + PIDOFFSET)*FOUR_MB), (void*)VADDRPROGPAGE, USWFLAGS);
  // map_page((fd_avail)*FOUR_MB, 0x08048000, 0x83);
  // check the ELF magic number:
  // byte 1: 0x7F
  // byte 2: 0x45
  // byte 3: 0x4C
  // byte 4: 0x46
  if(buf[0] == 0x7F && buf[1] == 0x45 && buf[2] == 0x4C && buf[3] == 0x46){
    // is executable
    // (27th index is MSB)
    // shift to concatenate buf[24-27] by right shifting with multiples of 8 (size of each byte)
    // put into EIP for context switch, evenutually
    firstInstruct = (buf[27] << 24) | (buf[26] << 16) | (buf[25] << 8) | (buf[24]);
  }
  else{
    // is not executable
    return -1;
  }
  // load the file contents into virtual memory
  // printf("%s \n", fnameSave);
  load(temp, (uint8_t*)VADDRPROGIMG);

  // fill tss
  tss.ss0 = KERNEL_DS;
  // tss.esp0 = 0x7FFFFC; //save esp of first new kernel stack
  tss.esp0 = KERNELSTACK - (pid * EIGHTKB) - KSTACKOFFSET;

  
  // flush the tlb, page change
  flush_tlb();

  // do not want to switch to user when creating the terminals
  // if creating terminals, reset parent and state to -1
  if (switch_after_creation_temp){
    switch_to_user_mode(firstInstruct);
  }
  else{
    PCB_array[pid].parent_pid = -1;
    PCB_array[pid].state = -1;
  }

  // child processes will jump here on halt return
  __asm__ __volatile__
  (
   "ret_label:"
  );


  return 0;
}
