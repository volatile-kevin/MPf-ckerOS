// char de(); //divide error
// char db(); //reserved  (reserved)
// char nmi(); //nonmaskable external interrupt
// char bp(); //breakpoint
// char of(); //overflow
// char br(); //bound range exceeded
// char ud(); //invalid opcode
// char nm(); //device not available
// char df(); //double fault --> return zero always
// char cpso(); //coprocessor segment overrun (reserved)
// char ts(); //invalid tss
// char np(); //segment not present
// char ssf(); //stack segment fault
// char gp(); //general protection
// char pf(); //page fault
// char ir(); //intel reserved do not use
// char mf(); //x87 FPU error
// char ac(); //alignment check ---> return zero
// char mc(); //machine check
// char xf(); //simd floating point exception

// // void fill_idt(unsigned long long * idt_addr);
// // void setup_idt(void* handler_address, int irq_num);
// void setup_idt_inplace();
