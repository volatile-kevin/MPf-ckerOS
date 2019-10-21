#include "paging.h"
#include "types.h"

//MACROS
#define ONE_KB 1024
#define FOUR_KB 4096
#define INITIAL_PAGEDIR_VAL 0x00000002
#define INITIAL_PAGETABLE_ATTRIBUTE 3

//Paging arrays
extern uint32_t pageDirectory[ONE_KB] __attribute__((aligned(FOUR_KB)));
extern uint32_t pageTable[ONE_KB] __attribute__((aligned(FOUR_KB)));

void init_paging();
void* get_physaddr(void* virtualaddr);
void* map_page(void* physaddr, void* virtualaddr, unsigned int flags);
void flush_tlb();

