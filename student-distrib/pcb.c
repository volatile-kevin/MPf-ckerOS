#include "filesys.h"
#include "terminal.h"
#include "rtc.h"
#include "lib.h"
#include "pcb.h"

#define KERNEL_PAGE_BOTTOM 0x800000
#define NUMPCBS 6
#define MAXFDTLEN 8
#define SKIPSTDINSTDOUT 2


uint8_t* start_addr;
uint32_t numdentries;
uint32_t numinodes;
uint32_t numblocks;
dentry_t* startdentry;
file_desc_t* fdt[MAXFDTLEN];


// this function initializes the file filesystem
// it reads the boot block and fills global variables with the boot block meta data
// parameters: address of the bootblock in memory
// returns: none
// side effect: initialized the file system
void pcb_filesys_init(uint32_t* mod_start){
  start_addr = (uint8_t*)mod_start;
  numdentries = *mod_start;
  numinodes = *(mod_start + INODES_OFFSET);
  numblocks = *(mod_start + NUMBLOCKS_OFFSET);
  startdentry = (dentry_t*)(start_addr + SIZEOF_DENTRY);
}

// structs that define what open, close, read, write should be used
// based on file type
fop rtc_struct = {rtc_open,
                  rtc_close,
                  rtc_read,
                  rtc_write};

fop dir_struct = {dir_open,
                 dir_close,
                 dir_read,
                 dir_write};

fop file_struct = {file_open,
                   file_close,
                   file_read,
                   file_write};

fop terminal_struct = {terminal_open,
                       terminal_close,
                       terminal_read,
                       terminal_write};


// initializes all PCBs with std_in and std_out
// sets the rest of the entries in the FDT as available
// parameters: none
// returns: none
// side effect: all PCBs is intialized correct
void init_PCB(){
    int i, j;

    //iterate through and populate each pcb, then load into kernel page at 8 kb offsets
    for (j = 0; j < NUMPCBS; j++){
        for (i = 0; i < NUM_FDT_ENTRIES; i++){
            // set -1 to say that it is currently not set
            PCB_array[j].fd_table[i].flags_arr[0] = -1;
        }
        PCB_array[j].pcb_in_use = -1;
        PCB_array[j].process_id = j;
        PCB_array[j].state = -1;
        //stdin members
        PCB_array[j].fd_table[0].jump_start_idx = &terminal_struct;
        PCB_array[j].fd_table[0].file_pos = 0;
        PCB_array[j].fd_table[0].inode_number = 0;
        PCB_array[j].fd_table[0].fileType = 3;
        //stdout members
        PCB_array[j].fd_table[1].jump_start_idx = &terminal_struct;
        PCB_array[j].fd_table[1].file_pos = 0;
        PCB_array[j].fd_table[1].inode_number = 0;
        PCB_array[j].fd_table[1].fileType = 3;
      }
}

// gets next available PCB
// parameters: none
// returns: the pid (process id) of the next available PCB
// side effect: the PCB with the given pid is set as in use
int getPID(){
  int i;
  for(i = 0; i < NUMPCBS; i++){
    if(PCB_array[i].pcb_in_use == -1) {
        PCB_array[i].pcb_in_use = 0;
        return i;
    }
  }
  return -1;
}

// Gets the next available entry in a PCB's FDT given a pid
// parameters: process_id: id of PCB to look for available entry in FDT
// returns: next available fd
// side effect: that fd is set as in use
int get_fdAvail(int process_id){
  int i;
  for(i = SKIPSTDINSTDOUT; i < NUM_FDT_ENTRIES; i++){
    if(PCB_array[process_id].fd_table[i].flags_arr[0] == -1){
      PCB_array[process_id].fd_table[i].flags_arr[0] = 0;
      return i;
    }
  }
  return -1;
}

// Insert a new entry into a PCB's FDT
// parameters: filename: name of file to add to FDT
// returns: 0 on success, else -1 for failure
// side effect: entry placed into PDB's FDT
int insert_fdt(const uint8_t* filename){
  int i = file_open(filename);
  if(i == -1){
    return -1;
  }
  int j, retval;
  int currPid, currFD;
  dentry_t dent;
  dentry_t* currDentry = &dent;

  // find active PCB
  for(j = 0; j < NUMPCBS; j++){
    if(PCB_array[j].state == 0){
      currPid = j;
    }
  }
  currFD = get_fdAvail(currPid);
  if(currFD == -1){
    return -1;
  }
  retval = read_dentry_by_name(filename, currDentry);
  if(retval == -1){
    return -1;
  }
  // if file was able to be opened
  if(i != -1){

    // set the data in the FDT entry based on the opened file
    PCB_array[currPid].fd_table[currFD].fileType = currDentry->fileType;
    PCB_array[currPid].fd_table[currFD].inode_number = currDentry->inodeNum;
  }
  else{
    return i;
  }
  // set the correct fops stuct depending on the file type
  switch(PCB_array[currPid].fd_table[currFD].fileType)
  {
    case 0:
      PCB_array[currPid].fd_table[currFD].jump_start_idx = &rtc_struct;
      break;
    case 1:
      PCB_array[currPid].fd_table[currFD].jump_start_idx = &dir_struct;
      break;
    case 2:
      PCB_array[currPid].fd_table[currFD].jump_start_idx = &file_struct;
      break;
  }
  return currFD;
}

int remove_fd_entry(int fd){
  int j;
  int currPid;
  for(j = 0; j < NUMPCBS; j++){
    if(PCB_array[j].state == 0){
      currPid = j;
    }
  }
  if(PCB_array[currPid].fd_table[fd].flags_arr[0] == 0){
    PCB_array[currPid].fd_table[fd].flags_arr[0] = -1;
    return 0;
  }
  return -1;
}

/**get_current_PCB
 * returns a pointer to the current 'in use' PCB
 * returns null if none are found
 */
PCB_struct* get_current_PCB(){
  int i = 0;
  for (; i < NUMPCBS; i++){
    if (PCB_array[i].state == 0){
       // printf("Current PCB: %d\n\n\n\n\n", i);
       return &PCB_array[i];
    }
  }
  return NULL;
}
