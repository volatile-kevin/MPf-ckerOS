#include "lib.h"
#include "syscall.h"
#include "terminal.h"
#include "execute.h"
#include "filesys.h"
#include "pcb.h"
#include "halt.h"

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

    return terminal_read(fd, buf, nbytes);
}
// write syscall wrapper
// returns whatever it calls returns
int32_t sys_write (int32_t fd, const void* buf, int32_t nbytes){
    return terminal_write (fd, buf, nbytes);
}
// open syscall wrapper
// returns whatever it calls returns
int32_t sys_open (const uint8_t* filename){
  return insert_fdt(filename);
}
// close syscall wrapper
// returns whatever it calls returns
int32_t sys_close (int32_t fd){
    return 0;
}

// error handler
void syscall_handler(){
    printf("syscall parameters invalid\n");
}
