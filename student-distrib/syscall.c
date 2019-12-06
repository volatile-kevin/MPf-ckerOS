#include "lib.h"
#include "syscall.h"
#include "terminal.h"
#include "execute.h"
#include "filesys.h"
#include "pcb.h"
#include "halt.h"
#include "paging.h"
#include "schedule.h"

#define TEMPBUFSIZES 1024

int currPid;
int currFD;
// halt syscall wrapper
// returns whatever it calls returns
int32_t sys_halt (uint8_t status){
    return halt(status);
}
// execute syscall wrapper
// returns whatever it calls returns
int32_t sys_execute (const uint8_t* command){
    return execute(command, 1);
}
// read syscall wrapper
// returns whatever it calls returns
int32_t sys_read (int32_t fd, void* buf, int32_t nbytes){
    if(fd < 0 || fd > 7 || fd == 1){
      return -1;
    }
    if(fd == 0){
      return terminal_read(fd, buf, nbytes);
    }
    else{
      if(PCB_array[currPid].fd_table[currFD].present == -1 || currFD == -1){
        return -1;
      }
      return PCB_array[currPid].fd_table[currFD].jump_start_idx->read(fd, buf, nbytes);
      // return (PCB_array[currPid].fd_table[fd].jump_start_idx)
    }
}
// write syscall wrapper
// returns whatever it calls returns
int32_t sys_write (int32_t fd, const void* buf, int32_t nbytes){
    if(fd < 0 || fd > 7 || fd == 0){
      return -1;
    }
    if(fd == 1){
      return terminal_write (fd, buf, nbytes);
    }
    else{
      if(PCB_array[currPid].fd_table[currFD].present == -1 || currFD == -1){
        return -1;
      }
      return PCB_array[currPid].fd_table[currFD].jump_start_idx->write(fd, buf, nbytes);
    }

}
// open syscall wrapper
// returns whatever it calls returns
int32_t sys_open (const uint8_t* filename){
  if(filename[0] == 0){
    return -1;
  }
  currFD = insert_fdt(filename);
  currPid = terminals[cur_terminal].curr_process;
  return currFD;
}

// close syscall wrapper
// returns whatever it calls returns
int32_t sys_close (int32_t fd){
    int retval;
    if(fd < 3 || fd > 7){
      return -1;
    }
    retval = remove_fd_entry(fd);
    if(retval == 0){
        currFD--;
    }
    return retval;
}

//Maps a page in the user program to the video memory in physical memory
int32_t sys_vidmap(uint8_t** screen_start){
  if(screen_start == (uint8_t**)0x0 || (screen_start >= (uint8_t**)0x400000 && screen_start < (uint8_t**)0x800000)){
    return -1;
  }
    return vid_map(screen_start);
}

// error handler
void syscall_handler(){
    printf("syscall parameters invalid\n");
}

int32_t sys_getargs(uint8_t* buf, int32_t nbytes){
    PCB_struct* curpcb = get_current_PCB(); //get a pointer to the current active pcb
    terminal_getargs(curpcb->args, TEMPBUFSIZES);
    if (curpcb->args[0] == '\0')
        return -1;
    memcpy(buf, curpcb->args, nbytes);
    return 0;
}
