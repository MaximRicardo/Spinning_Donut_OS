/* This assumes the i386-elf-gcc compiler is being used. If not, the sizes could be wrong */

#pragma once

typedef char int8_t;
typedef unsigned char uint8_t;

typedef short int16_t;
typedef unsigned short uint16_t;

typedef int int32_t;
typedef unsigned int uint32_t;


typedef uint32_t size_t;    /* With a flat memory model, the maximum addressable size is pow(2, 32) */
