#include <stdint.h>

#include "MemFuncs.h"
#include "Print.h"
#include "MemoryMap.h"

#define m_MEMORY_MAP_START (0x7e00)

struct MemoryMapEntry memoryMap[256];
uint16_t nMemoryMapEntries;

static void MemoryMap_RemoveEntry(size_t entryI) {

    size_t j;

    /* Shift the rest of the entries down one index */
    for (j = entryI+1; j < nMemoryMapEntries; j++) {

        memoryMap[j-1] = memoryMap[j];

    }

    /* Decrement the table size */
    --nMemoryMapEntries;

}

static void MemoryMap_GetRidOf0SizeEntries() {

    size_t i;

    for (i = 0; i < nMemoryMapEntries; i++) {

        if (memoryMap[i].lengthLo > 0) continue;

        /* Remove the entry if it's size is 0 */
        MemoryMap_RemoveEntry(i);

    }

}

static void MemoryMap_LimitTo4GiB() {

    size_t i;
    for (i = 0; i < nMemoryMapEntries; i++) {

        if (memoryMap[i].baseHi > 0) {
            /* If the base of the region is above 4 294 967 296 bytes (4GiB), remove the entry. This is true if the upper 32 bits of the base address are above 0 */
            MemoryMap_RemoveEntry(i);
            continue;       /* Move to the next entry */
        }

        if (memoryMap[i].lengthHi > 0) {
            /* If the length of the region is above 4 GiB, limit the length to be 4 GiB - 1B */
            memoryMap[i].lengthHi = 0;
            memoryMap[i].lengthLo = 0xffffffff;
        }

        if (memoryMap[i].baseLo + memoryMap[i].lengthLo < memoryMap[i].baseLo) {
            /* If memoryMap[i].baseLo + memoryMap[i].lengthLo, overflowed, then limit the length of the region to put the end of the region at 0xffffffff */
            memoryMap[i].lengthLo = 0xffffffff - memoryMap[i].baseLo;   /* Since memoryMapEnd = memoryMapBase + memoryMapLength, and memoryMapEnd must equal 0xffffffff, solving for memoryMapLength is: memoryMapLength = memoryMapEnd -  memoryMapBase = 0xffffffff - memoryMapBase */
        }

    }

}

static void MemoryMap_MergeSameTypeNeighbors() {

    /* Breaks if any regions are overlapping */

    size_t i;

    for (i = 0; i < nMemoryMapEntries-1U; i++) {

        /* Skip the entry if it's neighbor is a different type or isn't contiguous with the current entry */
        if (memoryMap[i+1].type != memoryMap[i].type || memoryMap[i+1].baseLo != memoryMap[i].baseLo+memoryMap[i].lengthLo) continue;

        /* Add the entries sizes together */
        memoryMap[i].lengthLo += memoryMap[i+1].lengthLo;
        
        /* Remove the neighboring entry */
        MemoryMap_RemoveEntry(i+1);

    }

}

/* Returns a pointer to the memory memoryMap */
struct MemoryMapEntry* MemoryMap_Load(size_t *nEntries) {

    /* Get the size of the memory memoryMap table */
    nMemoryMapEntries = *(uint16_t*)m_MEMORY_MAP_START;

    /* Load in the table */
    memcpy(memoryMap, (void*)(m_MEMORY_MAP_START+2), nMemoryMapEntries * sizeof(struct MemoryMapEntry));

    /* Get rid of entries of 0 size */
    MemoryMap_GetRidOf0SizeEntries();

    /* Limit the entries to be within the 4GiB of memory */
    MemoryMap_LimitTo4GiB();

    /* Merge neighboring regions of same type */
    MemoryMap_MergeSameTypeNeighbors();

    *nEntries = nMemoryMapEntries;
    return memoryMap;

}

void MemoryMap_Print() {

    size_t i;

    kPrintf("Number of entries = %u\n", nMemoryMapEntries);

    for (i = 0; i < nMemoryMapEntries; i++) {

        kPrintf("Region at 0x%x-0x%x. Size = 0x%x. Type = ", memoryMap[i].baseLo, memoryMap[i].baseLo+memoryMap[i].lengthLo-1, memoryMap[i].lengthLo);

        if (memoryMap[i].type == 1) kPuts("\"Free\"");
        else if (memoryMap[i].type == 2) kPuts("\"Reserved\"");
        else if (memoryMap[i].type == 3) kPuts("\"ACPI Reclaimable\"");
        else if (memoryMap[i].type == 4) kPuts("\"ACPI NVS\"");
        else if (memoryMap[i].type == 5) kPuts("\"Bad Memory\"");
        else kPrintf("\"Undefined\": %d\n", memoryMap[i].type);

    }

}

uint32_t MemoryMap_GetTopOfLargestFreeEntry() {

    uint32_t largestSize = 0;
    uint32_t finalAddress = 0;

    size_t i;

    for (i = 0; i < nMemoryMapEntries; i++) {

        /* Ignore non-free entries */
        if (memoryMap[i].type != 1) continue;

        if (memoryMap[i].lengthLo > largestSize) {

            largestSize = memoryMap[i].lengthLo;
            finalAddress = memoryMap[i].lengthLo + memoryMap[i].baseLo - 1;

        }

    }

    return finalAddress;

}




