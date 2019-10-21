#ifndef PAGING_H_INCLUDED
#define PAGING_H_INCLUDED

#include "types.h"

//MACROS
#define ONE_KB 1024
#define FOUR_KB 4096
#define INITIAL_PAGEDIR_VAL 0x00000002
#define INITIAL_PAGETABLE_ATTRIBUTE 3

void init_paging(void);
//void* get_physaddr(void* virtualaddr);
//void* map_page(void* physaddr, void* virtualaddr, unsigned int flags);
extern void flush_tlb(void);
extern void enable_paging(void);

////Paging arrays
extern uint32_t page_directory[ONE_KB] __attribute__((aligned(FOUR_KB)));
extern uint32_t page_table[ONE_KB] __attribute__((aligned(FOUR_KB)));


#endif
