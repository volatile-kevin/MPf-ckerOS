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


/* Interrupt masks to determine which interrupts are enabled and disabled */
uint8_t master_mask = 0xFF; /* IRQs 0-7  */
uint8_t slave_mask = 0xFF;  /* IRQs 8-15 */

/* Initialize the 8259 PIC */
void i8259_init(void) {
  // unsigned char mask1, mask2;
  // save masks
  // master_mask = inb(PIC1_DATA);
  // slave_mask = inb(PIC2_DATA);
  // mask the master
  outb(0xFF, PIC1_DATA);
  // mask the slave
  outb(0xFF, PIC2_DATA);

  // starts initialization sequence in casade mode
  outb(ICW1, PIC1_COMMAND);
  outb(ICW1, PIC2_COMMAND);

  outb(ICW2_MASTER, PIC1_COMMAND);
  outb(ICW2_SLAVE, PIC2_COMMAND);

  outb(ICW3_MASTER, PIC1_COMMAND);
  outb(ICW3_SLAVE, PIC2_COMMAND);

  outb(ICW4, PIC1_COMMAND);
  outb(ICW4, PIC2_COMMAND);

}

/* Enable (unmask) the specified IRQ */
void enable_irq(uint32_t irq_num) {
  uint32_t mask = ~(1 << irq_num);

  master_mask &= mask;
  slave_mask &= mask;

  if(irq_num & 8){
    outb(slave_mask, PIC2_DATA);
  }
  else{
    outb(master_mask, PIC2_DATA);
  }
}

/* Disable (mask) the specified IRQ */
void disable_irq(uint32_t irq_num) {
  uint32_t mask = (1 << irq_num);

  master_mask |= mask;
  slave_mask |= mask;

  if(irq_num & 8){
    outb(slave_mask, PIC2_DATA);
  }
  else{
    outb(master_mask, PIC2_DATA);
  }
}

/* Send end-of-interrupt signal for the specified IRQ */
void send_eoi(uint32_t irq_num) {
  if(irq_num >= 8){
    outb(EOI | irq_num, PIC2_COMMAND);
  }
  outb(EOI | irq_num, PIC1_COMMAND);
}
