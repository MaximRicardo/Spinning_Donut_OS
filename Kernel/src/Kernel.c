#include <stdint.h>

#include "LowLevelIO.h"
#include "MemFuncs.h"
#include "Screen.h"
#include "Print.h"
#include "IDT.h"
#include "PIC.h"
#include "MemoryMap.h"
#include "Donut.h"

#define m_PIT_FREQUENCY (1193181)   /* PIT clock frequency in Hz */

#define m_PIT_IRQ_DELAY (0.001f)    /* How many seconds between each PIT interrupt */
#define m_PIT_RESET_TIME (m_PIT_IRQ_DELAY * (float)m_PIT_FREQUENCY)

void KernelMain(size_t stackStart) {

    PIC_Initialize();   /* Initialize the PIC */
    IDT_Load();     /* Load in the interrupt descriptor table */

    /* Set the PIT to send an interrupt approximately every millisecond */
    outb(0x43, 0x34);   /* 0x00110100 = 0b00110100 */
    outb(0x40, (uint16_t)m_PIT_RESET_TIME & 0xff);
    outb(0x40, ((uint16_t)m_PIT_RESET_TIME >> 8) & 0xff);
 
    __asm__ volatile ("sti");   /* Enable interrupts again */

    kPrintf("Stack starts at 0x%x\n", stackStart & -16);

    MemoryMap_Load(&nMemoryMapEntries);       /* Load in the memory map again, this time at the new stack */

    MemoryMap_Print();

    kPuts("\n\n");
    Donut();

    return;

}
