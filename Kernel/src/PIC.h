#pragma once

#include "stdint.h"

#include "LowLevelIO.h"

#define PIC1            0x20    /* IO base address for master PIC */
#define PIC2            0xA0    /* IO base address for slave PIC */
#define PIC1_COMMAND    PIC1
#define PIC1_DATA       (PIC1+1)
#define PIC2_COMMAND    PIC2
#define PIC2_DATA       (PIC2+1)


#define PIC_EOI         0x20    /* End of interrupt command code */


#define ICW1_ICW4       0x01    /* Indicates that ICW4 will be present */
#define ICW1_SINGLE     0x02    /* Single (cascade) mode */
#define ICW1_INTERVAL4  0x04    /* Call address interval 4 (8) */
#define ICW1_LEVEL      0x08    /* Level triggered (edge) mode */
#define ICW1_INIT       0x10    /* Initialization - required! */
#define ICW1_INIT_ICW4  0x11

#define ICW2_MASTER_OFFSET  0x20
#define ICW2_SLAVE_OFFSET   0x28

#define ICW3_MASTER_SLAVE_IRQ   0x4
#define ICW3_SLAVE_MASTER_CASCADE   0x2

#define ICW4_8086       0x01    /* 8086/88 (MCS-80/85) mode */
#define ICW4_AUTO       0x02    /* Auto (normal) EOI */
#define ICW4_BUF_SLACE  0x08    /* Buffered mode/slave */
#define ICW4_BUF_MASTER 0x0c    /* Buffered mode/master */
#define ICW4_SFNM       0x10    /* Special fully nested (not) */


#define PIC_READ_IRR    0x0a    /* OCW3 irq ready next CMD read */
#define PIC_READ_ISR    0x0b    /* OCW3 irq service next CMD read */


void PIC_SendEOI(uint8_t irq);

/*
offset1 - Vector offset for the Master PIC
offset2 - Vector offset for the Slave PIC
*/
void PIC_Remap(int offset1, int offset2);

void PIC_Disable();

void IRQ_SetMask(uint8_t irqLine);
void IRQ_ClearMask(uint8_t irqLine);

/* Returns the combined value of the cascaded PICs irq request register */
uint16_t PIC_GetIRR();
/* Returns the combined value of the cascaded PICs in-service register */
uint16_t PIC_GetISR();

/* Intitializes the PIC and mask all interrupts excepth the timer and keyboard*/
void PIC_Initialize();
