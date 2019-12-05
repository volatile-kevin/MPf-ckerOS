.globl rtc_asm
.globl keyboard_asm
.globl pit_asm
.globl pit_return
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


 #
 #      rtc_asm
 #   DESCRIPTION: Linkage that stores register values before calling our c function
 #   INPUTS: none
 #   OUTPUTS: none
 #   RETURN VALUE: none
 #   SIDE EFFECTS: rtc is called everytime it ticks (1052 Hz)
 #
rtc_asm:
    pushal
    # call keyboard_handler, storing all registers
    call rtc_handler
    popal
    iret

pit_asm:
    pushal
    call pit_handler

pit_return:
    popal
    iret


