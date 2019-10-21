.globl enable_paging
.globl flush_tlb

enable_paging:
    push %ebp
    movl %esp, %ebp
    push %eax
    movl 8(%esp), %eax
    movl %eax, %cr3
    movl %cr0, %eax
    orl $0x80000000, %eax
    movl %eax, %cr0
    pop %eax
    movl %ebp, %esp
    popl %ebp
    ret


/**
 * flushes tlb so we can see the changes to the paging structures
 * ===========
 * No parameters
 * No return value
 * flshes the tlb
 */
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
