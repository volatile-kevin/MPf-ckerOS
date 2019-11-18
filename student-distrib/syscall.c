#include "lib.h"
#include "syscall.h"
#include "terminal.h"
#include "execute.h"
#include "filesys.h"
#include "pcb.h"
#include "halt.h"
#include "paging.h"

#define NUMPCBS 6

int currPid;
int currFD;
int bytescounter = 0;
// halt syscall wrapper
// returns whatever it calls returns
int32_t sys_halt (uint8_t status){
    return halt(status);
}
// execute syscall wrapper
// returns whatever it calls returns
int32_t sys_execute (const uint8_t* command){
    return execute(command);
}
// read syscall wrapper
// returns whatever it calls returns
int32_t sys_read (int32_t fd, void* buf, int32_t nbytes){
    if(fd == 0){
      return terminal_read(fd, buf, nbytes);
    }
    else{
      bytescounter += PCB_array[currPid].fd_table[currFD].jump_start_idx->read(fd, buf, nbytes);
      // return (PCB_array[currPid].fd_table[fd].jump_start_idx)
    }
    if(bytescounter >= 512){
      return 0;
    }
    else{
      return 32;
    }
}
// write syscall wrapper
// returns whatever it calls returns
int32_t sys_write (int32_t fd, const void* buf, int32_t nbytes){
    if(fd == 1){
      return terminal_write (fd, buf, nbytes);
    }
    else{
      return PCB_array[currPid].fd_table[currFD].jump_start_idx->write(fd, buf, nbytes);
    }

}
// open syscall wrapper
// returns whatever it calls returns
int32_t sys_open (const uint8_t* filename){
  currFD = insert_fdt(filename);
  int j;
  for(j = 0; j < NUMPCBS; j++){
    if(PCB_array[j].state == 0){
      currPid = j;
    }
  }
  return currFD;
  // PCB_array[currPid].fd_table[fd].jump_start_idx
}
// close syscall wrapper
// returns whatever it calls returns
int32_t sys_close (int32_t fd){
    remove_fd_entry(fd);
    return 0;
}

//Maps a page in the user program to the video memory in physical memory
int32_t sys_vidmap(uint8_t** screen_start){
    return vid_map(screen_start);
}

// error handler
void syscall_handler(){
    printf("syscall parameters invalid\n");
}
