#include "SizedInts.h"

extern void InterruptDefault();
extern void Interrupt0();
extern void Interrupt32();
extern void Interrupt33();

/*
SEL_VALUE is always 8.
The 0th and 1st bits are the privilege level of the selector
The 2nd bit is 0 for GDT and 1 for LDT
The rest of the bits are the index of the code segment, which is 1
*/
#define SEL_VALUE 8     /* 0b1000 */

#define FLAGS_VALUE 0x8e    /* 0b10001110 */

#define CREATE_INTERRUPT_ENTRY(i, base)     idt[i].baseLo = (uint16_t)((uint32_t)base & 0xffff);\
                                            idt[i].baseHi = (uint16_t)(((uint32_t)base >> 16) & 0xffff);\
                                            idt[i].sel = SEL_VALUE;\
                                            idt[i].always0 = 0;\
                                            idt[i].flags = FLAGS_VALUE

/* Creates an entry with all zeroes */
#define CREATE_ZERO_ENTRY(i)    idt[i].baseLo = 0;\
                                idt[i].baseHi = 0;\
                                idt[i].sel = 0;\
                                idt[i].always0 = 0;\
                                idt[i].flags = 0

/*Defines an IDT entry*/
struct IDTEntry {

    uint16_t baseLo;
    uint16_t sel;       /* The kernel segment goes here */
    uint8_t always0;    /* This will always be set to 0 */
    uint8_t flags;
    uint16_t baseHi;

} __attribute__((packed));


/* Declare an IDT of 256 entries. */
struct IDTEntry idt[256];

/* Creates all the entries for the idt */
void IDT_Create() {

    /* Division Error entry */
    CREATE_INTERRUPT_ENTRY(0, &Interrupt0);

    /* PIT timer interrupt entry */
    CREATE_INTERRUPT_ENTRY(32, &Interrupt32);

    /* Keyboard interrupt entry */
    CREATE_INTERRUPT_ENTRY(33, &Interrupt33);

}

void IDT_Load() {

    size_t i;

    /* Initialize all the interrupt entries */

    for (i = 0; i < 32; i++) {  /* Use the default interrupt handler for all the cpu expection interrupts by default */
        CREATE_INTERRUPT_ENTRY(i, &InterruptDefault);
    }

    for (i = 32; i < 256; i++) {    /* Default the rest to 0 */
        /*CREATE_ZERO_ENTRY(i);*/
        CREATE_INTERRUPT_ENTRY(i, &InterruptDefault);
    }

    IDT_Create();

    /* Copy every entry to the Interrupt Descriptor Table located at 0x0000 */
    for (i = 0; i < 256; i++) {

        /* Copying over base lo */
        *(uint8_t*)(i*8) = (uint8_t)(idt[i].baseLo & 0xff);
        *(uint8_t*)(i*8+1) = (uint8_t)((idt[i].baseLo >> 8) & 0xff);

        /* Copying over sel */
        *(uint8_t*)(i*8+2) = (uint8_t)(idt[i].sel & 0xff);
        *(uint8_t*)(i*8+3) = (uint8_t)((idt[i].sel >> 8) & 0xff);

        /* Copying over always 0 */
        *(uint8_t*)(i*8+4) = idt[i].always0;

        /* Copying over flags */
        *(uint8_t*)(i*8+5) = idt[i].flags;

        /* Copying over baseHi */
        *(uint8_t*)(i*8+6) = (uint8_t)(idt[i].baseHi & 0xff);
        *(uint8_t*)(i*8+7) = (uint8_t)((idt[i].baseHi >> 8) & 0xff);

    }

}
