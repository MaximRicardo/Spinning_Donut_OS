/* Contains functions for manipulating memory */ 

#pragma once

#include "SizedInts.h"

void* memcpy(void* dest, void* src, size_t n);
void* memset(void* ptr, uint8_t value, size_t n);
