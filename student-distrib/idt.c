#include "idt.h"
#include "x86_desc.h"
#include "keyboard.h"
//this code sets up an entry into the Interrupt Descriptor Table
//your IDT must contain entries for exceptions, a few interrupts, system calls

//		31....16     16 15   
/*|Offset 31...16|P |DPL |0D110|000|    | */
#define pointersSize 21

unsigned long long * addr;

//static pointers[pointersSize];
typedef char (* const shit) (void);
static shit pointers[] = {de, db, nmi, bp, of, br, ud, nm, df, cpso, ts, np, ssf, gp, pf, ir, 
                        mf, ac, mc, xf, tc, keyboard};

void fill_idt(unsigned long long * idt_addr){
    addr = idt_addr;
    unsigned i;
    for (i = 0; i  < pointersSize; i++){
        setup_idt(pointers[i], i);
    }
}


void setup_idt(void* handler_address, int irq_num){
    uint32_t idt_upper;
    uint32_t idt_lower;
    /* Set Offset 31:16 Present DPL */
    idt_upper = ((unsigned long)handler_address & 0xffff0000) | (1 << 15) | (3 << 13) | 0x0E00; /* Size, other ‘1’ bits */
    /* Segment Selector Offset 15:00 */
    idt_lower = (KERNEL_CS << 16) | (unsigned long long)handler_address & 0xffff;
    /* Fill in the entry in the IDT */
    addr[irq_num] = ((unsigned long)(idt_upper) << 32) | idt_lower;
}

char de(){printf("ERROR\n");return 0;} //divide error
char db(){printf("ERROR\n");return 0;} //reserved  (reserved)
char nmi(){printf("ERROR\n");return 0;} //nonmaskable external interrupt
char bp(){printf("ERROR\n");return 0;} //breakpoint
char of(){printf("ERROR\n");return 0;} //overflow
char br(){printf("ERROR\n");return 0;} //bound range exceeded
char ud(){printf("ERROR\n");return 0;} //invalid opcode
char nm(){printf("ERROR\n");return 0;} //device not available
char df(){printf("ERROR\n");return 0;} //double fault --> return zero always
char cpso(){printf("ERROR\n");return 0;} //coprocessor segment overrun (reserved)
char ts(){printf("ERROR\n");return 0;} //invalid tss
char np(){printf("ERROR\n");return 0;} //segment not present
char ssf(){printf("ERROR\n");return 0;} //stack segment fault
char gp(){printf("ERROR\n");return 0;} //general protection
char pf(){printf("ERROR\n");return 0;} //page fault
char ir(){printf("ERROR\n");return 0;} //intel reserved do not use
char mf(){printf("ERROR\n");return 0;} //x87 FPU error
char ac(){printf("ERROR\n");return 0;} //alignment check ---> return zero
char mc(){printf("ERROR\n");return 0;} //machine check
char xf(){printf("ERROR\n");return 0;} //simd floating point exception
char tc(){printf("ERROR\n");return 0;} //timing chip

// call our keyboard handler
char keyboard(){
    unsigned int keyboard_data;
    keyboard_data = inb(0x60);
    keyboard_handler(keyboard_data);
    return 0;
} //divide error
