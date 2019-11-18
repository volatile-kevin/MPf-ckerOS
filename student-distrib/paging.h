#ifndef PAGING_H_INCLUDED
#define PAGING_H_INCLUDED

#include "types.h"

//MACROS
#define ONE_KB 1024
#define FOUR_KB 4096 //Default size of a page
#define FOUR_MB 4194304 //Size of a big page
#define VIDEO_MEM_INDEX 0xB8
#define INITIAL_PAGEDIR_VAL 0x00000002
#define INITIAL_PAGETABLE_ATTRIBUTE 0x2
#define INITIAL_PAGETABLE 0x3
#define INITIAL_VIDEOMEM_ATTRIBUTE 0x3
#define INITIAL_KERNEL_ATTRIBUTE 0x83 //Sets the size to 1 so the page is 4MB
#define VIDEO_MEM       0xB8000 //defined in lib.c
#define USER_MAP_LOC 138412032 //Right after the process page


void init_paging(void);
//void* get_physaddr(void* virtualaddr);
void map_page(void* physaddr, void* virtualaddr, unsigned int flags);
extern void flush_tlb(void);
extern void enable_paging(void* pagediraddress);
int32_t vid_map(uint8_t** address);


////Paging arrays
extern uint32_t page_directory[ONE_KB] __attribute__((aligned(FOUR_KB)));
extern uint32_t page_table[ONE_KB] __attribute__((aligned(FOUR_KB)));
extern uint32_t user_video_page_table[ONE_KB] __attribute__((aligned(FOUR_KB)));


#endif
