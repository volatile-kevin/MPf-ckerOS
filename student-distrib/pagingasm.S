.globl enable_paging
.globl flush_tlb

enable_paging:
    push %ebp
    movl %esp, %ebp
    #Set cr3 to the address of the page directory
    push %eax
    movl 8(%ebp), %eax
    movl %eax, %cr3
    #Set cr4 to enable 4MB pages
    movl %cr4, %eax
    orl $0x00000010, %eax #Setting the flag to enable 4mb pages
    movl %eax, %cr4
    #Set cr0 to enable paging
    movl %cr0, %eax
    orl $0x80000000, %eax #setting the flag to enable paging
    movl %eax, %cr0
    pop %eax
    movl %ebp, %esp
    popl %ebp
    ret


#/**
# * flushes tlb so we can see the changes to the paging structures
# * ===========
# * No parameters
# * No return value
# * flshes the tlb
# */
flush_tlb:
    push %ebp
    movl %esp, %ebp
    push %eax
    movl %cr3, %eax
    movl %eax, %cr3
    pop %eax
    movl %ebp, %esp
    popl %ebp
    ret
