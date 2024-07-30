section .text
global _start

[bits 32]
[extern KernelMain]
[extern puts]
[extern printf]
[extern MemoryMap_Load]
[extern MemoryMap_GetTopOfLargestFreeEntry]

_start:

    push ebp
    mov ebp, esp

    sub esp, 8  ;size_t nMemoryMapEntries, struct MemoryMapEntry* memoryMap
    
    ;Load in the idt
    lidt [IDT_Descriptor]

    ;Determine if the SSE is available
    mov eax, 0x1
    cpuid
    test edx, 1<<25
    jz SSENotAvailable

    ;SSE is available

    ; init FPU
    fninit
    fldcw [fcw]

    ; enable SSE
    mov eax, cr0
    and al, ~0x04
    or al, 0x22
    mov cr0, eax
    mov eax, cr4
    or ax, 0x600
    mov cr4, eax

    ;Get the memory map
    lea eax, [ebp-4]  ;Number of entries is stored at ebp-4
    push eax
    call MemoryMap_Load
    add esp, 4

    ;Store the pointer to the memory map in ebp-8
    mov [ebp-8], eax

    ;Get the address of the top of the largest free region in memory
    call MemoryMap_GetTopOfLargestFreeEntry

    ;Assign esp to that address, moving the entire stack to that area. This means all previous local variables are lost
    mov esp, eax


StackMoved:

    ;Create a new stack frame
    push ebp
    mov esp, ebp
    
    ;Pass the top of the largest free region to the kernel
    push eax        ;Pass the start of the stack to the kernel
    call KernelMain
    add esp, 4

    jmp HaltLoop


fcw: dw 0x037f

IDT_Descriptor:
    dw 2048
    dd 0

HaltLoop:
    hlt
    jmp HaltLoop


SSENotAvailableStr: db "SSE not available. This OS requires that the SSE is available", 0xa, 0x0

SSENotAvailable:

    push SSENotAvailableStr
    call puts
    add esp, 4
    
    jmp HaltLoop











