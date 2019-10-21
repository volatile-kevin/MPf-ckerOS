/* i8259.c - Functions to interact with the 8259 interrupt controller
 * vim:ts=4 noexpandtab
 */

#include "i8259.h"
#include "lib.h"


#define PIC1		0x20		/* IO base address for master PIC */
#define PIC2		0xA0		/* IO base address for slave PIC */
#define PIC1_COMMAND	PIC1
#define PIC1_DATA	(PIC1+1)
#define PIC2_COMMAND	PIC2
#define PIC2_DATA	(PIC2+1)
#define SIZEOFPIC1 8
#define MSLINK 2
#define MMASK 0xFB
#define SMASK 0xFF
/* Interrupt masks to determine which interrupts are enabled and disabled */
uint8_t master_mask = MMASK; /* IRQs 0-7  */
uint8_t slave_mask = SMASK;  /* IRQs 8-15 */

/* Initialize the 8259 PIC */
void i8259_init(void) {

  // mask the master
  outb(master_mask, PIC1_DATA);
  // mask the slave
  outb(slave_mask, PIC2_DATA);

  // starts initialization sequence in casade mode
  // send ICW1 to 4 to PICs
  outb(ICW1, PIC1_COMMAND);
  outb(ICW2_MASTER, PIC1_DATA);
  outb(ICW3_MASTER, PIC1_DATA);
  outb(ICW4, PIC1_DATA);

  outb(ICW1, PIC2_COMMAND);
  outb(ICW2_SLAVE, PIC2_DATA);
  outb(ICW3_SLAVE, PIC2_DATA);
  outb(ICW4, PIC2_DATA);

  outb(master_mask, PIC1_DATA);
  // mask the slave
  outb(slave_mask, PIC2_DATA);

}

/* Enable (unmask) the specified IRQ 
*  This is done by ANDing with the NOT of 1 << irq_num. 
*  essentially, the old mask is saved
*/
void enable_irq(uint32_t irq_num) {
  uint32_t mask = ~(1 << irq_num);

 // master_mask &= mask;
  //slave_mask &= mask;

  if(irq_num >= SIZEOFPIC1){
    uint32_t unmask = ~(1 << (irq_num - SIZEOFPIC1));
    slave_mask &= unmask;
    outb(slave_mask, PIC2_DATA);
  }
  else{
    master_mask &= mask;
    outb(master_mask, PIC1_DATA);
  }
}

/* Disable (mask) the specified IRQ 
*  This is done by ORing with 1 << irq_num
*/
void disable_irq(uint32_t irq_num) {
  uint32_t mask = (1 << irq_num);

  master_mask |= mask;
  slave_mask |= mask;

  if(irq_num & SIZEOFPIC1){
    outb(slave_mask, PIC2_DATA);
  }
  else{
    outb(master_mask, PIC1_DATA);
  }
}

/* Send end-of-interrupt signal for the specified IRQ 
*  We must send EOI | 2 to the master PIC so that it knows 
*  slave is receiving 
*/
void send_eoi(uint32_t irq_num) {
  if(irq_num >= SIZEOFPIC1){
    outb(EOI | (irq_num - SIZEOFPIC1), PIC2_COMMAND);
    outb(EOI | MSLINK, PIC1_COMMAND);
  }
  else
    outb(EOI | irq_num, PIC1_COMMAND);
}
