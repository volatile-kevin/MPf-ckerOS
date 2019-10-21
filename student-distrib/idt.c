#include "idt2.h"
#include "x86_desc.h"
#include "keyboard.h"
#include "lib.h"

void de(){printf("ERROR 1\n"); while(1);} //divide error
void db(){printf("ERROR 2\n");while(1);return;} //reserved  (reserved)
void nmi(){printf("ERROR 3\n");while(1);return;} //nonmaskable external interrupt
void bp(){printf("ERROR 4\n");while(1);return;} //breakpoint
void of(){printf("ERROR 5\n");while(1);return;} //overflow
void br(){printf("ERROR 6\n");while(1);return;} //bound range exceeded
void ud(){printf("ERROR 7\n");while(1);return;} //invalid opcode
void nm(){printf("ERROR 8\n");while(1);return;} //device not available
void df(){printf("ERROR 9\n");while(1);return;} //double fault --> return zero always
void cpso(){printf("ERROR 10\n");while(1);return;} //coprocessor segment overrun (reserved)
void ts(){printf("ERROR 11\n");while(1);return;} //invalid tss
void np(){printf("ERROR 12\n");while(1);return;} //segment not present
void ssf(){printf("ERROR 13\n");while(1);return;} //stack segment fault
void gp(){printf("ERROR 14\n");while(1);return;} //general protection
void pf(){printf("ERROR 15\n");while(1);return;} //page fault
void ir(){printf("ERROR 16\n");while(1);return;} //intel reserved do not use
void mf(){printf("ERROR 17\n");while(1);return;} //x87 FPU error
void ac(){printf("ERROR 18\n");while(1);return;} //alignment check ---> return zero
void mc(){printf("ERROR 19\n");while(1);return;} //machine check
void xf(){printf("ERROR 20\n");while(1);return;} //simd floating point exception

// call our keyboard handler
void keyboard(){
    printf("inside keyboard");
    // read keyboard data off port
    // keyboard_data = inb(0x60);
    keyboard_handler();
    while (1);
    return;
} //divide error


void setup_idt_inplace(){
    int i;
    printf("we got here");

    for (i = 0; i < 20; i++){
        idt[i].seg_selector = KERNEL_CS;
        idt[i].dpl = 0;
        idt[i].reserved0 = 0;
        idt[i].reserved1 = 1;
        idt[i].reserved2 = 1;
        idt[i].reserved3 = 1;
        idt[i].reserved4 = 0;
        idt[i].present = 1;
    }

    // static shit pointers[] = {de, db, nmi, bp, of, br, ud, nm, df, cpso, ts, np, ssf, gp, pf, ir, 
//                         mf, ac, mc, xf};
        idt[0x21].seg_selector = KERNEL_CS;
        idt[0x21].dpl = 0;
        idt[0x21].reserved0 = 0;
        idt[0x21].reserved1 = 1;
        idt[0x21].reserved2 = 1;
        idt[0x21].reserved3 = 0;
        idt[0x21].reserved4 = 0;
        idt[0x21].present = 1;

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
        SET_IDT_ENTRY(idt[0x21], keyboard);

        printf("we got to the bottom here");
}
