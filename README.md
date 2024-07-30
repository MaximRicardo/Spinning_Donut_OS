# Donut OS

Donut OS is an operating system designed for specifically one thing. A spinning donut.

build.sh will create a binary file called OS.bin in the Binaries folder, which will contain the bootable operating system.
run.sh will use QEMU to emulate the operating system.

This OS requires that the target cpu and computer has support for: SSE, 32-bit Protected Mode, and a BIOS.

Don't run this on real hardware. No promises this won't cause your computer to die.

Written entirely in Ansi C (+ stdint, stdbool and size_t), and some x86 assembly.

![image](https://github.com/user-attachments/assets/70b7fc7e-0b12-4ba4-97ff-993c9f5c5f61)
