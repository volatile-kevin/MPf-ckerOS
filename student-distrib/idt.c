#include "idt.h"
#include "x86_desc.h"
#include "keyboard.h"
#include "lib.h"
#include "linkage.h"
#include "syscall_asm.h"

#define SYSCALL_NUM 128 // 0x80
#define NUM_IDT_ENTRIES 47 // current number of entries in our IDT we have defined

void de() {
    printf("ERROR 1\n");
    while (1);
} //divide error
void db() {
    printf("ERROR 2\n");
    while (1);
    return;
} //reserved  (reserved)
void nmi() {
    printf("ERROR 3\n");
    while (1);
    return;
} //nonmaskable external interrupt
void bp() {
    printf("ERROR 4\n");
    while (1);
    return;
} //breakpoint
void of() {
    printf("ERROR 5\n");
    while (1);
    return;
} //overflow
void br() {
    printf("ERROR 6\n");
    while (1);
    return;
} //bound range exceeded
void ud() {
    printf("ERROR 7\n");
    while (1);
    return;
} //invalid opcode
void nm() {
    printf("ERROR 8\n");
    while (1);
    return;
} //device not available
void df() {
    printf("ERROR 9\n");
    while (1);
    return;
} //double fault --> return zero always
void cpso() {
    printf("ERROR 10\n");
    while (1);
    return;
} //coprocessor segment overrun (reserved)
void ts() {
    printf("ERROR 11\n");
    while (1);
    return;
} //invalid tss
void np() {
    printf("ERROR 12\n");
    while (1);
    return;
} //segment not present
void ssf() {
    printf("ERROR 13\n");
    while (1);
    return;
} //stack segment fault
void gp() {
    printf("ERROR 14\n");
    while (1);
    return;
} //general protection
void pf() {
    printf("ERROR 15\n");
    printf("Cur terminal = %d\n", cur_terminal);
    while (1);
    return;
} //page fault
void ir() {
    printf("ERROR 16\n");
    while (1);
    return;
} //intel reserved do not use
void mf() {
    printf("ERROR 17\n");
    while (1);
    return;
} //x87 FPU error
void ac() {
    printf("ERROR 18\n");
    while (1);
    return;
} //alignment check ---> return zero
void mc() {
    printf("ERROR 19\n");
    while (1);
    return;
} //machine check
void xf() {
    printf("ERROR 20\n");
    while (1);
    return;
} //simd floating point exception
void gen_purp() {
    printf("WACK\n");
    while (1);
    return;
}

void set_entry(int i) {
    idt[i].seg_selector = KERNEL_CS;
    idt[i].dpl = 0;
    idt[i].reserved0 = 0;
    idt[i].reserved1 = 1;
    idt[i].reserved2 = 1;
    idt[i].reserved3 = 0;
    idt[i].present = 1;
    idt[i].size = 1;

    if (i == SYSCALL_NUM) {
        idt[i].dpl = 3;
        idt[i].reserved3 = 1;
    }
}


/*
 * setup_idt_inplace
 *   DESCRIPTION: Sets up the IDT with the correct values associates them with their corresponding function
 *   INPUTS: none
 *   OUTPUTS: none
 *   RETURN VALUE: none
 *   SIDE EFFECTS: IDT is setup and ready for use
 */
void setup_idt_inplace() {
    int i;

    // set each entry that we are concerned with, with the correct bit values
    for (i = 0; i < NUM_IDT_ENTRIES; i++) {
        set_entry(i);
    }
    set_entry(SYSCALL_NUM);//set system call handler

    // associate the set values in IDT with the function that handles when the exception/interrupt is raised
    SET_IDT_ENTRY(idt[0], de);
    SET_IDT_ENTRY(idt[1], db);
    SET_IDT_ENTRY(idt[2], nmi);
    SET_IDT_ENTRY(idt[3], bp);
    SET_IDT_ENTRY(idt[4], of);
    SET_IDT_ENTRY(idt[5], br);
    SET_IDT_ENTRY(idt[6], ud);
    SET_IDT_ENTRY(idt[7], nm);
    SET_IDT_ENTRY(idt[8], df);
    SET_IDT_ENTRY(idt[9], cpso);
    SET_IDT_ENTRY(idt[10], ts);
    SET_IDT_ENTRY(idt[11], np);
    SET_IDT_ENTRY(idt[12], ssf);
    SET_IDT_ENTRY(idt[13], gp);
    SET_IDT_ENTRY(idt[14], pf);
    SET_IDT_ENTRY(idt[15], ir);
    SET_IDT_ENTRY(idt[16], mf);
    SET_IDT_ENTRY(idt[17], ac);
    SET_IDT_ENTRY(idt[18], mc);
    SET_IDT_ENTRY(idt[19], xf);
    SET_IDT_ENTRY(idt[20], gen_purp);
    SET_IDT_ENTRY(idt[21], gen_purp);
    SET_IDT_ENTRY(idt[22], gen_purp);
    SET_IDT_ENTRY(idt[23], gen_purp);
    SET_IDT_ENTRY(idt[24], gen_purp);
    SET_IDT_ENTRY(idt[25], gen_purp);
    SET_IDT_ENTRY(idt[26], gen_purp);
    SET_IDT_ENTRY(idt[27], gen_purp);
    SET_IDT_ENTRY(idt[28], gen_purp);
    SET_IDT_ENTRY(idt[29], gen_purp);
    SET_IDT_ENTRY(idt[30], gen_purp);
    SET_IDT_ENTRY(idt[31], gen_purp);

    SET_IDT_ENTRY(idt[32], pit_asm);
    SET_IDT_ENTRY(idt[33], keyboard_asm);
    SET_IDT_ENTRY(idt[34], gen_purp);
    SET_IDT_ENTRY(idt[35], gen_purp);
    SET_IDT_ENTRY(idt[36], gen_purp);
    SET_IDT_ENTRY(idt[37], gen_purp);
    SET_IDT_ENTRY(idt[38], gen_purp);
    SET_IDT_ENTRY(idt[39], gen_purp);
    SET_IDT_ENTRY(idt[40], rtc_asm);
    SET_IDT_ENTRY(idt[41], gen_purp);
    SET_IDT_ENTRY(idt[42], gen_purp);
    SET_IDT_ENTRY(idt[43], gen_purp);
    SET_IDT_ENTRY(idt[44], gen_purp);
    SET_IDT_ENTRY(idt[45], gen_purp);
    SET_IDT_ENTRY(idt[46], gen_purp);
    SET_IDT_ENTRY(idt[47], gen_purp);
    SET_IDT_ENTRY(idt[0x80], syscall_handler_asm);
}
