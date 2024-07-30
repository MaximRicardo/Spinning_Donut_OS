#!/bin/bash

#qemu-system-x86_64 -drive format=raw,file=Binaries/OS.bin
qemu-system-x86_64 -drive format=raw,file=Binaries/OS.bin,index=0,if=floppy -m 4G
