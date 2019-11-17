.globl syscall_handler_asm

# doSomething (arg1, arg2, arg3, arg4);
# pushl arg4
# pushl arg3
# pushl arg2
# pushl arg1
# call  doSomething

# Parameters (convention)
# EAX – system call number
# EBX – 1st arg
# ECX – 2nd arg
# EDX – 3rd arg
# Return value in EAX (not all calls return)

#
# int32_t halt (uint8_t status);
#
syscall_halt:
    # TODO: might have to do some bit shifting still with %ebx
    andl $0xF, %ebx
    pushl %ebx # uint8_t status
    call sys_halt

    # should not return to the caller

#
# int32_t execute (const uint8 t* command);
#
syscall_execute:
    pushal      # save registers

    pushl %ebx # const uint8_t* command
    call sys_execute
    popl %ebx     # return from execute.c
    popal         # pop old stack data
#    push %eax
#    pushl (%esp)
    movl $0, %eax
    iret

#
#  int32_t read (int32 t fd, void* buf, int32 t nbytes);
#
syscall_read:
    cmpl $-1, %edx
    je fail
    cmpl $-1, %ecx
    je fail
    cmpl $-1, %ebx
    je fail

    pushl %edx # int32_t nbytes
    pushl %ecx # void* buf
    pushl %ebx # int32_t fd
    call sys_read
    popl %ebx  # pop into don't care regs
    popl %ebx
    popl %ebx

    iret


#
# int32 t write (int32_t fd, const void* buf, int32_t nbytes);
#
syscall_write:
    pushl %edx # int32_t nbytes
    pushl %ecx # void* buf
    pushl %ebx # int32_t fd
    call sys_write
    popl %ebx  # pop into don't care regs
    popl %ebx
    popl %ebx

    iret

#
# int32 t open (const uint8_t* filename);
#
syscall_open:
    pushl %ebx # const uint8_t* filename
    call sys_open
    popl %ebx

    iret

#
# int32_t close (int32_t fd);
#
syscall_close:
    pushl %ebx # int32_t fd
    call sys_close
    popl %ebx

    iret


 #
 #   syscall_handler_asm
 #   DESCRIPTION: Linkage that passes register values to our C function
 #   INPUTS: input values from stack
 #   OUTPUTS: none
 #   RETURN VALUE: none
 #   SIDE EFFECTS: syscall (Execute, halt, open, close, read, write) is passed appropriate params
 #

syscall_handler_asm:
    # cmp $1, %eax
    # je syscall_halt
    # cmp $2, %eax
    # je syscall_execute
    # cmp $3, %eax
    # je syscall_read
    # cmp $4, %eax
    # je syscall_write
    # cmp $5, %eax
    # je syscall_open
    # cmp $6, %eax
    # je syscall_close
    subl $1, %eax
    jmp *jump_table(,%eax,4)

fail:
    # shouldn't get here
    call syscall_handler
    # popal
    iret

jump_table:
  .long syscall_halt, syscall_execute, syscall_read, syscall_write, syscall_open, syscall_close
