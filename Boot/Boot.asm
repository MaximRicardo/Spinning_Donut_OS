[org 0x7c00]

CODE_SEG equ GDT_CodeDescriptor - GDT_Start
DATA_SEG equ GDT_DataDescriptor - GDT_Start

%define KERNEL_LOCATION 0x1000
%define KERNEL_SECTORS_TO_READ 40

%define MEMORY_MAP_LOCATION 0x7e00

segment .text

    mov [BOOT_DISK], dl
    
    ;Setup the segment registers
    xor ax, ax
    mov es, ax
    mov ds, ax
    
    ;Setup the stack
    mov bp, 0x8000
    mov sp, bp


    ;Allocate space for a counter, this will keep track of how many times the program has attempted to load the kernel, initialize this variable to 0
    push 0
    ;A counter for the number of entries in the memory map
    push 0

LoadingKernel:
    ;Setup a pointer to the kernel in bx
    mov bx, KERNEL_LOCATION
    
    ;Load in the kernel from disk
    mov ah, 2
    mov al, KERNEL_SECTORS_TO_READ
    mov ch, 0
    mov cl, 2
    mov dh, 0
    mov dl, [BOOT_DISK]
    int 0x13

    jc KernelLoadError  ;If carry flag is set, there was an error
    cmp al, KERNEL_SECTORS_TO_READ
    jne KernelLoadError ;If the incorrect number of sectors were read, then there was an error

    ;Enable A20 if it isn't enabled already
    call CheckA20
    cmp  ax, 0
    jne  EnableA20Done
    call SetA20BIOS
    call CheckA20
    cmp  ax, 0
    jne  EnableA20Done
    call SetA20Keyboard
    call CheckA20
    cmp  ax, 0
    jne  EnableA20Done
    call SetA20FastGate
    call CheckA20
    xchg bx, bx
    cmp  ax, 0
    jne  EnableA20Done

EnableA20Fail:
    ;If A20 line couldn't be enabled, halt the program
    jmp HaltLoop

EnableA20Done:

    ;Load in the memory map
    mov di, MEMORY_MAP_LOCATION+2
    xor ebx, ebx

GetMemoryMapLoop:

    mov edx, 0x534D4150
    mov eax, 0xe820
    mov ecx, 24
    int 0x15

    jc GetMemoryMapLoopEnd
    cmp eax, 0x534D4150
    jne GetMemoryMapLoopEnd

    inc word[bp-4]

    cmp ebx, 0
    je GetMemoryMapLoopEnd

    add di, 24

    jmp GetMemoryMapLoop

GetMemoryMapLoopEnd:

    mov ax, word[bp-4]
    mov [MEMORY_MAP_LOCATION], ax

    ;Before switching to protected mode, switch over to 640*480 monochrome VGA mode
    mov ah, 0
    mov al, 0x11
    int 0x10

    ;Switch to protected mode
    cli
    lidt [IDT_Descriptor]
    lgdt [GDT_Descriptor]
    ;Change the least significant bit of cr0 to 1
    mov eax, cr0
    or eax, 1
    mov cr0, eax
    ;Far jump to code
    jmp CODE_SEG:StartProtectedMode
    nop ;Make completely sure the instruction prefetch is invalidated
    nop

KernelLoadError:
    inc word[bp-2]      ;Retry loading the kernel 4 times
    cmp word[bp-2], 4
    jl LoadingKernel

    ;Halt forever
    ;jmp HaltLoop   Not necessary as the program will automatically fall into the halt loop

HaltLoop:
    hlt
    jmp HaltLoop

SetA20BIOS:
    mov ax, 0x2401
    int 0x15
    ret

SetA20Keyboard:
    cli                         ; Disable interrupts

    call    Wait8042Command     ; When controller ready for command
    mov     al,0xAD             ; Send command 0xad (disable keyboard).
    out     0x64,al

    call    Wait8042Command     ; When controller ready for command
    mov     al,0xD0             ; Send command 0xd0 (read from input)
    out     0x64,al

    call    Wait8042Data        ; When controller has data ready
    in      al,0x60             ; Read input from keyboard
    push    eax                 ; ... and save it

    call    Wait8042Command     ; When controller is ready for command
    mov     al,0xD1             ; Set command 0xd1 (write to output)
    out     0x64,al            

    call    Wait8042Command     ; When controller is ready for command
    pop     eax                 ; Write input back, with bit #2 set
    or      al,2
    out     0x60,al

    call    Wait8042Command     ; When controller is ready for command
    mov     al,0xAE             ; Write command 0xae (enable keyboard)
    out     0x64,al

    call    Wait8042Command     ; Wait until controller is ready for command

    sti                         ; Enable interrupts
    ret

    
Wait8042Command:
    in      al,0x64
    test    al,2
    jnz     Wait8042Command
    ret


Wait8042Data:
    in      al,0x64
    test    al,1
    jz      Wait8042Data
    ret


SetA20FastGate:
    in al, 0x92
    or al, 2
    out 0x92, al
    ret


CheckA20:
    pushf                           ; Save registers that
    push ds                         ; we are going to
    push es                         ; overwrite.
    push di
    push si

    cli                             ; No interrupts, please

    xor ax, ax                      ; Set es:di = 0000:0500
    mov es, ax
    mov di, 0x0500

    mov ax, 0xffff                  ; Set ds:si = ffff:0510
    mov ds, ax
    mov si, 0x0510

    mov al, byte es:[di]            ; Save byte at es:di on stack.
    push ax                         ; (we want to restore it later)

    mov al, byte ds:[si]            ; Save byte at ds:si on stack.
    push ax                         ; (we want to restore it later)

    mov byte es:[di], 0x00          ; [es:di] = 0x00
    mov byte ds:[si], 0xFF          ; [ds:si] = 0xff

    cmp byte es:[di], 0xFF          ; Did memory wrap around?

    pop ax
    mov byte ds:[si], al            ; Restore byte at ds:si

    pop ax
    mov byte es:[di], al            ; Restore byte at es:di

    mov ax, 0
    je .CheckA20Exit                ; If memory wrapped around, return 0.

    mov ax, 1                       ; else return 1.

.CheckA20Exit:
    pop si                          ; Restore saved registers.
    pop di
    pop es
    pop ds
    popf
    ret


BOOT_DISK: db 0


IDT_Descriptor:     ;A dummy IDT
    dw 0
    dd 0

GDT_Start:                          ;Must be at the end of real mode code
    GDT_nullDescriptor:
        dd 0    ;four bytes of 0
        dd 0    ;four bytes of 0
    GDT_CodeDescriptor:
        dw 0xffff   ;First 16 bits of the limit
        dw 0x0      ;First 24 bits of the base
        db 0x0
        db 0b10011010   ;Descriptor and type flags
        db 0b11001111   ;Other flags and last four bits of limit
        db 0            ;Last 8 bits of base
    GDT_DataDescriptor:
        dw 0xffff   ;First 16 bits of the limit
        dw 0x0      ;First 24 bits of the base
        db 0x0
        db 0b10010010   ;Descriptor and type flags
        db 0b11001111   ;Other flags and last four bits of limit
        db 0            ;Last 8 bits of base
    GDT_End:

    GDT_Descriptor:
        dw GDT_End - GDT_Start - 1  ;Size
        dd GDT_Start                ;Start


[bits 32]
;##########################################################
;#              32 bit code starts from here              #
;##########################################################

StartProtectedMode:
    mov ax, DATA_SEG    ;Setup segment registers and the stack
    mov ds, ax
    mov ss, ax
    mov es, ax
    mov fs, ax
    mov gs, ax
    mov ebp, 0x80000
    mov esp, ebp

    jmp KERNEL_LOCATION ;Jump to the kernel


times 510-($-$$) db 0
dw 0xaa55
