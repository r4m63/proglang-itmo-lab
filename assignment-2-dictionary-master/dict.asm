%include "lib.inc"

section .text

global find_word



find_word:
    xor     rax, rax
    sub     rsp, 8;
.loop:
    push    rsi
    push    rdi
    lea     rsi, [rsi + 8]
    
    call    string_equals
    pop     rdi
    pop     rsi
    test    rax, rax
    jnz     .exit_found
    cmp     qword[rsi], 0
    je      .exit_not_found
   
    mov     rsi, [rsi]
    jmp     .loop
.exit_found:
    mov     rax, rsi
    add     rax, 8;
.exit_not_found:
    add     rsp, 8
    ret
