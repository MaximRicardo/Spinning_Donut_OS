#include "Print.h"
#include "PIC.h"
#include "LowLevelIO.h"

extern uint32_t pitIntCount;
uint32_t pitIntCount = 0;


void EndOfInterrupt() {

    /* Send end of interrupt message to both PICs */
    outb(PIC1_COMMAND, PIC_EOI);
    outb(PIC2_COMMAND, PIC_EOI);

}


/*
intAddress  - The location in memory the cpu was executing at when the interrupt happened
*/
void InterruptDefaultHandler(void* intAddress) {

    printf("Exception encountered at address 0x%p.\n", intAddress);

}

/*
intAddress  - The location in memory the cpu was executing at when the interrupt happened
*/
void Interrupt0Handler(void* intAddress) {

    printf("Error: Divide Error exception encountered at address 0x%p.\n", intAddress);

}


void Interrupt32Handler() {

    ++pitIntCount;
    PIC_SendEOI(32);    

}


void Interrupt33Handler() {

    /* printf("Keyboard interrupt!\n"); */
    
    if (inb(0x64) & 0x01) {
        /*
        int scanCode = inb(0x60);
        printf("scan code = %d\n", scanCode); */
        inb(0x60);
    }

    PIC_SendEOI(33);

}
