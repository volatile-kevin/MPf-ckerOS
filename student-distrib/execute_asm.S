

.globl switch_to_usr
.globl flush_tlb
# context switch
switch_to_usr:

  cli

  movw $0x2B, %ax # USER_DS
  movw %ax, %ds # move into all registers
  movw %ax, %es
  movw %ax, %fs
  movw %ax, %gs

  # movl %esp, %eax
  pushl $0x2B # USER_DS
  # pushl %eax
  pushl $0x83FFFFC
  pushfl # push flags
  popl %edx         # pop the flags and mask them with 0x200, then put them back on the stack
  orl $0x200, %edx
  pushl %edx
  pushl $0x23 # USER_CS
  pushl 8(%ebp) # push the entrypoint (parameter)
  iret
