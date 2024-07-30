[bits 32]

[extern InterruptDefaultHandler]
[extern Interrupt0Handler]
[extern Interrupt32Handler]
[extern Interrupt33Handler]

section .text
global InterruptDefault
global Interrupt0
global Interrupt32
global Interrupt33

;Default interrupt that unitialized interrupt entries will use
InterruptDefault:

    cli

    ;Handle the interrupt
    push dword [esp]    ;Push the address of where the cpu was executing when the interrupt happened
    call InterruptDefaultHandler
    add esp, 2          ;Clean up the arguments

    ;Halt the program forever
    jmp HaltLoop


;#DE    Division Error
Interrupt0:

    cli

    ;Handle the interrupt
    push dword [esp]    ;Push the address of where the cpu was executing when the interrupt happened
    call Interrupt0Handler
    add esp, 2          ;Clean up the arguments

    ;Halt the program forever
    jmp HaltLoop


;Interrupt called by the pit timer
Interrupt32:

    cli

    pushad

    ;Handle the interrupt
    call Interrupt32Handler

    popad
    iret


;Interrupt called by the keyboard
Interrupt33:
    
    cli

    pushad

    ;Handle the interrupt
    call Interrupt33Handler
    
    popad
    iret




HaltLoop:
    hlt
    jmp HaltLoop
