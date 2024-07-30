#pragma once

struct MemoryMapEntry {

    uint32_t baseLo;    /* Address of the region */
    uint32_t baseHi;    /* Ignored since only the first 4G will be used */

    uint32_t lengthLo;  /* Length of the region in bytes */
    uint32_t lengthHi;  /* Ignored since only the first 4G will be used */

    uint32_t type;      /* Type of the region */

    uint32_t unused;

}__attribute__((packed));

extern struct MemoryMapEntry memoryMap[256];
extern uint16_t nMemoryMapEntries;

/* Returns a pointer to the memory map */
struct MemoryMapEntry* MemoryMap_Load();

void MemoryMap_Print();

uint32_t MemoryMap_GetTopOfLargestFreeEntry();
