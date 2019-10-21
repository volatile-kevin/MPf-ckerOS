/* paging.c - sets up paging*/
#include "paging.h"
#include "types.h"

//For now we will use static arrays. Will try to add dynamic memory allocation later
uint32_t page_directory[ONE_KB] __attribute__((aligned(FOUR_KB)));
uint32_t page_table[ONE_KB] __attribute__((aligned(FOUR_KB)));

/**
 * init_paging
 * ===========
 * No parameters
 * No return value
 * Initializes the page directory and the first page table
 */

void init_paging(void){
    //As described in the doc we need to mark all entries as not present (except for the video memory)
    unsigned int i = 0;
    for(i = 0; i < ONE_KB; i++){
        // This sets the following flags to the pages:
        //   Supervisor: Only kernel-mode can access them
        //   Write Enabled: It can be both read from and written to
        //   Not Present: The page table is not present
        page_directory[i] = INITIAL_PAGEDIR_VAL;

        // This sets the following flags to the page table entries:
        //   Supervisor: Only kernel-mode can access them
        //   Write Enabled: It can be both read from and written to
        //   Not Present: The page is present
        page_table[i] = (i*FOUR_KB) | INITIAL_PAGETABLE_ATTRIBUTE;
    }
    //This sets the entry in the page table for video memory

    page_table[VIDEO_MEM_INDEX] |= 3;


    //Add the first page table to the page directory
    page_directory[0] = ((unsigned int)page_table) | INITIAL_PAGETABLE_ATTRIBUTE;
    //The second page is the page for the kernel. We map this to 4MB since the doc told us to
    page_directory[1] = FOUR_MB | INITIAL_KERNEL_ATTRIBUTE;


    //We have to use inline assembly for this part because we cannot access CR3 or CR0 or CR4 in C
    enable_paging(page_directory);
}
/**
 * get_physaddr
 * ===========
 * param: the virtual address to be converted
 * returns: The physical address
 * Converts the virtual address to the physical address
 */
//void* get_physaddr(void* virtualaddr){
//    unsigned long pdindex = (unsigned long)virtualaddr >> 22;
//    unsigned long ptindex = (unsigned long)virtualaddr >> 12 & 0x03FF;
//
//    unsigned long * pd = (unsigned long *)0xFFFFF000;
//    // Here you need to check whether the PD entry is present.
//    if(page_directory[pdindex] & 0x) {
//        unsigned long *pt = ((unsigned long *) 0xFFC00000) + (0x400 * pdindex);
//        // Here you need to check whether the PT entry is present.
//
//
//        return (void *) ((pt[ptindex] & ~0xFFF) + ((unsigned long) virtualaddr & 0xFFF));
//    }
//}

//**
// * map_page
// * ===========
// * param: the physical address, the virtual address to be converted, the flags
// * returns: The physical address
// * maps the virtual address to the physical address
// */
//void map_page(void* physaddr, void* virtualaddr, unsigned int flags){
//    // Make sure that both addresses are page-aligned.
//    if(!((unsigned long)physaddr%FOUR_KB) && !((unsigned long)virtualaddr%FOUR_KB)){
//        return;
//    }
//    unsigned long pdindex = (unsigned long)virtualaddr >> 22;
//    unsigned long ptindex = (unsigned long)virtualaddr >> 12 & 0x03FF;
//
//    unsigned long * pd = (unsigned long *)0xFFFFF000;
//    // Here you need to check whether the PD entry is present.
//    // When it is not present, you need to create a new empty PT and
//    // adjust the PDE accordingly.
//
//    unsigned long * pt = ((unsigned long *)0xFFC00000) + (0x400 * pdindex);
//    // Here you need to check whether the PT entry is present.
//    // When it is, then there is already a mapping present. What do you do now?
//
//    pt[ptindex] = ((unsigned long)physaddr) | (flags & 0xFFF) | 0x01; // Present
//
//    // Now you need to flush the entry in the TLB
//    // or you might not notice the change.
//    flush_tlb();
//}


