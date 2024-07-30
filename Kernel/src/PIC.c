#include "PIC.h"

void PIC_SendEOI(uint8_t irq) {

    if (irq >= 8) outb(PIC2_COMMAND, PIC_EOI);

    outb(PIC1_COMMAND, PIC_EOI);

}


/*
offset1 - Vector offset for the Master PIC
offset2 - Vector offset for the Slave PIC
*/
void PIC_Remap(int offset1, int offset2) {

    uint8_t a1, a2;

    a1 = inb(PIC1_DATA);        /* Save masks */
    a2 = inb(PIC2_DATA);

    outb(PIC1_COMMAND, ICW1_INIT | ICW1_ICW4);  /* Starts the initialization sequence (in cascade mode) */
    ioWait();
    outb(PIC2_COMMAND, ICW1_INIT | ICW1_ICW4);
    ioWait();
    outb(PIC1_DATA, offset1);   /* ICW2: Master PIC vector offset */
    ioWait();
    outb(PIC2_DATA, offset2);   /* ICW2: Slave PIC vector offset */
    ioWait();
    outb(PIC1_DATA, 4); /* ICW3: Tell the Master PIC that there is a Slave PIC at IRQ2 (0000 0100) */
    ioWait();
    outb(PIC2_DATA, 2); /* ICW3: Tell the Slave PIC it's cascade identity (0000 0010) */
    ioWait();

    outb(PIC1_DATA, ICW4_8086); /* ICW4: have the PICs use 8086 mode (and not 8080 mode) */
    ioWait();
    outb(PIC2_DATA, ICW4_8086);
    ioWait();

    outb(PIC1_DATA, a1);    /* Restore saved masks */
    outb(PIC1_DATA, a2);

}


void PIC_Disable() {

    outb(PIC1_DATA, 0xff);
    outb(PIC2_DATA, 0xff);

}


void IRQ_SetMask(uint8_t irqLine) {

    uint16_t port;
    uint8_t value;

    if (irqLine < 8) {
        port = PIC1_DATA;
    }
    else {
        port = PIC2_DATA;
        irqLine -= 8;
    }
    value = inb(port) | (1 << irqLine);
    outb(port, value);

}

void IRQ_ClearMask(uint8_t irqLine) {

    uint16_t port;
    uint8_t value;

    if (irqLine < 8) {
        port = PIC1_DATA;
    }
    else {
        port = PIC2_DATA;
        irqLine -= 8;
    }
    value = inb(port) & ~(1 << irqLine);
    outb(port, value);

}


static uint16_t PIC_GetIRQReg(int ocw3) {

    /* OCW3 to PIC CMD to get the register values. PIC2 is chained, and
        represents IRQs 8-15. PIC1 is IRQs 0-7, with 2 being the chain */
    outb(PIC1_COMMAND, ocw3);
    outb(PIC2_COMMAND, ocw3);
    return (inb(PIC2_COMMAND) << 8) | inb(PIC1_COMMAND);

}

/* Returns the combined value of the cascaded PICs irq request register */
uint16_t PIC_GetIRR() {

    return PIC_GetIRQReg(PIC_READ_IRR);

}

/* Returns the combined value of the cascaded PICs in-service register */
uint16_t PIC_GetISR() {

    return PIC_GetIRQReg(PIC_READ_ISR);

}


/* Intitializes the PIC and mask all interrupts excepth the timer and keyboard*/
void PIC_Initialize() {

    uint8_t masterMask = 0xFC;  /* Only timer and kbd */
    uint8_t slaveMask = 0xFF;   /* Disable the slave */

    /* Send control words and mask */
    outb(PIC1_COMMAND, ICW1_INIT_ICW4);
    ioWait();
    outb(PIC2_COMMAND, ICW1_INIT_ICW4);
    ioWait();

    outb(PIC1_DATA, ICW2_MASTER_OFFSET);
    ioWait();
    outb(PIC2_DATA, ICW2_SLAVE_OFFSET);
    ioWait();

    outb(PIC1_DATA, ICW3_MASTER_SLAVE_IRQ);
    ioWait();
    outb(PIC2_DATA, ICW3_SLAVE_MASTER_CASCADE);
    ioWait();

    outb(PIC1_DATA, ICW4_8086);
    ioWait();
    outb(PIC2_DATA, ICW4_8086);
    ioWait();

    outb(PIC1_DATA, masterMask);
    outb(PIC2_DATA, slaveMask);

}











