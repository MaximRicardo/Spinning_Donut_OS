#!/bin/bash

mkdir -p Binaries

nasm Boot/Boot.asm -f bin -o Binaries/Boot.bin

nasm Kernel/KernelEntry.asm -f elf -o Binaries/KernelEntry.o
nasm Kernel/src/Interrupts.asm -f elf -o Binaries/Interrupts.o
i386-elf-gcc -std=c99 -mgeneral-regs-only -Wall -Wextra -Wpedantic -pedantic -ffreestanding -m32 -O2 -c Kernel/Utils/src/LowLevelIO.c -o Binaries/LowLevelIO.o -IKernel/Utils/src -IKernel/src
i386-elf-gcc -std=c99 -Wall -Wextra -Wpedantic -pedantic -ffreestanding -m32 -O2 -march=pentium3 -mfpmath=sse -ftree-vectorize -c Kernel/Utils/src/MemFuncs.c -o Binaries/MemFuncs.o -IKernel/Utils/src -IKernel/src
i386-elf-gcc -std=c99 -mgeneral-regs-only -Wall -Wextra -Wpedantic -pedantic -ffreestanding -m32 -O2 -c Kernel/src/InterruptHandlers.c -o Binaries/InterruptHandlers.o -IKernel/Utils/src -IKernel/src
i386-elf-gcc -std=c99 -Wall -Wextra -Wpedantic -pedantic -ffreestanding -m32 -O2 -march=pentium3 -mfpmath=sse -ftree-vectorize -c Kernel/Utils/src/Print.c -o Binaries/Print.o -IKernel/Utils/src -IKernel/src
i386-elf-gcc -std=c99 -mgeneral-regs-only -Wall -Wextra -Wpedantic -pedantic -ffreestanding -m32 -O2 -c Kernel/src/IDT.c -o Binaries/IDT.o -IKernel/Utils/src -IKernel/src
i386-elf-gcc -std=c99 -mgeneral-regs-only -Wall -Wextra -Wpedantic -pedantic -ffreestanding -m32 -O2 -c Kernel/src/PIC.c -o Binaries/PIC.o -IKernel/Utils/src -IKernel/src
i386-elf-gcc -std=c99 -Wall -Wextra -Wpedantic -pedantic -ffreestanding -m32 -O2 -march=pentium3 -mfpmath=sse -ftree-vectorize -c Kernel/src/Chars.c -o Binaries/Chars.o -IKernel/Utils/src -IKernel/src
i386-elf-gcc -std=c99 -Wall -Wextra -Wpedantic -pedantic -ffreestanding -m32 -O2 -march=pentium3 -mfpmath=sse -ftree-vectorize -c Kernel/src/Kernel.c -o Binaries/Kernel.o -IKernel/Utils/src -IKernel/src
i386-elf-gcc -std=c99 -Wall -Wextra -Wpedantic -pedantic -ffreestanding -m32 -O2 -march=pentium3 -mfpmath=sse -ftree-vectorize -c Kernel/src/Donut.c -o Binaries/Donut.o -IKernel/Utils/src -IKernel/src
i386-elf-gcc -std=c99 -Wall -Wextra -Wpedantic -pedantic -ffreestanding -m32 -O2 -march=pentium3 -mfpmath=sse -ftree-vectorize -c Kernel/src/MemoryMap.c -o Binaries/MemoryMap.o -IKernel/Utils/src -IKernel/src

nasm Boot/Zeroes.asm -f bin -o Binaries/Zeroes.bin

i386-elf-ld -o Binaries/FullKernel.bin -Ttext 0x1000 Binaries/KernelEntry.o Binaries/Kernel.o Binaries/IDT.o Binaries/PIC.o Binaries/Interrupts.o Binaries/InterruptHandlers.o Binaries/Print.o Binaries/Chars.o Binaries/LowLevelIO.o Binaries/MemFuncs.o Binaries/MemoryMap.o Binaries/Donut.o --oformat binary

cat Binaries/Boot.bin Binaries/FullKernel.bin Binaries/Zeroes.bin > Binaries/OS.bin
