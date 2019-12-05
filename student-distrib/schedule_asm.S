.globl update_stack

update_stack:
   pushl %ebp
   movl  %esp, %ebp
   movl %ebp, %ebx
   movl 8(%ebx), %ebp
   movl 12(%ebx), %esp
   jmp pit_return




    