#include "MemFuncs.h"

void* memcpy(void* dest, void* src, size_t n) {

    uint8_t* destP = (uint8_t*)dest;
    uint8_t* srcP = (uint8_t*)src;

    size_t i;

    for (i = 0; i < n; i++) {
        destP[i] = srcP[i];
    }

    return dest;

}

void* memset(void* ptr, uint8_t value, size_t n) {

    uint8_t* p = (uint8_t*)ptr;

    size_t i;

    for (i = 0; i < n; i++) {
        p[i] = value;
    }

    return ptr;

}
