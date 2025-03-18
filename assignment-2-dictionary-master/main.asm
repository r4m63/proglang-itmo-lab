%include "word.inc"
%include "lib.inc"
%include "dict.inc"


%define BUFFER_LENGTH 255

section .rodata
    find_error: db "Label doesnt exists", 0
    read_error: db "Buffer was overflowed (max - 255)", 0

    
section .bss
    buffer:     resb BUFFER_LENGTH


section .text
global _start


_start:
    mov     rdi, buffer
    mov     rsi, BUFFER_LENGTH
    call    read_line
    mov     r12, rdx
    
    test    rax, rax
    jz      .exit_cannot_read
    
    mov     rdi, rax
    mov     rsi, first_word

    call    find_word
    test    rax, rax
    jz      .exit_cannot_found
    mov     rdi, rax
    add     rdi, r12
    inc     rdi
    call    print_string
    call    print_newline
    xor     rdi, rdi
    jmp     exit

.exit_cannot_found:
    mov     rdi, find_error
    jmp     .exit_fail
.exit_cannot_read:
    mov     rdi, read_error
.exit_fail:
    call    print_error
    call    print_newline
    mov     rdi, 1
    jmp     exit

