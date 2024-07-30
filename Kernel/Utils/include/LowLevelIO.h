#pragma once

#include "SizedInts.h"

void outb(uint32_t port, uint8_t val);
uint8_t inb(uint32_t port);

void ioWait();
