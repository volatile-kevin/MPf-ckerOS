
.globl keyboard_asm

 # 
 #      keyboard_asm
 #   DESCRIPTION: Linkage that stores register values before calling our c function
 #   INPUTS: none
 #   OUTPUTS: none
 #   RETURN VALUE: none
 #   SIDE EFFECTS: keyboard_handler is called to handle keyboard input
 #
keyboard_asm:
    pushal
    # call keyboard_handler, storing all registers
    call keyboard_handler
    popal
    iret

