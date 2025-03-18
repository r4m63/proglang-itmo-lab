section .text

%define SYS_READ_CODE           0
%define SYS_WRITE_CODE          1
%define SYS_EXIT_CODE           60

%define STDIN_CODE              0
%define STDOUT_CODE             1

%define RET_VAL_OK              0

%define TO_ASCII                48

%define ASCII_SPACE_CHAR        0x20      
%define ASCII_TAB_CHAR          0x9      
%define ASCII_NEW_LINE_CHAR     0xA      
%define ASCII_NULL_CHAR         0x0      

%define DECIMAIL_BASE           10


; +-----------+     +-----------+
; |   ARGS    |     |  RETURNS  |
; +-----------+     +-----------+
; |1   rdi    |     |1   rax    |
; |2   rsi    |     |2   rdx    |
; |3   rdx    |     +-----------+
; |4   rcx    |
; |5   r8     |
; |6   r9     |
; |7   stack  |
; +-----------+

; call-saved:   rbx, rbp, rsp, r12, r13, r14, r15
; caller-saved: other

; rax syscallnum
; rdi iodescript
; rsi ptr
; rdx len
;-rdi retval


;=====================================================================================
; Принимает код возврата и завершает текущий процесс
exit:
    mov rax, SYS_EXIT_CODE 
    syscall


;=====================================================================================
; Принимает указатель на нуль-терминированную строку, выводит её в stdout
print_string:
    push rdi                    ; str_ptr -> stack calling covencion
    call string_length          ; returns str_length -> rax 
    mov rdx, rax                ; length -> rdx
    pop rsi                     ; stack str_ptr -> rsi
    mov rax, SYS_WRITE_CODE     
    mov rdi, STDOUT_CODE        
    syscall
    ret



;=====================================================================================
; Принимает указатель на нуль-терминированную строку, возвращает её длину
string_length:
    xor rax, rax                ; counter
.start:
    cmp byte [rdi + rax], ASCII_NULL_CHAR 
    jz .end                     ; jmp if zero to end
    inc rax                     ; rax counter +=1, rax returns 
    jmp .start                 
.end:
    ret                       



;=====================================================================================
; Переводит строку (выводит символ с кодом 0xA)
print_newline:
    mov rdi, ASCII_NEW_LINE_CHAR



;=====================================================================================
; Принимает код символа и выводит его в stdout
print_char:
    xor rax, rax    
    sub rsp, 8                  ; ensure that the symbol is properly aligned on the stack
    push rdi                    ; arg in rdi -> stack
    mov rsi, rsp                ; char_ptr -> rsi msg_out_ptr
    mov rdx, 1                  ; char_len = 1
    mov rdi, STDOUT_CODE        
    mov rax, SYS_WRITE_CODE     
    syscall

    pop rdi
    add rsp, 8
    ret



;=====================================================================================
; Выводит беззнаковое 8-байтовое число в десятичном формате 
; Совет: выделите место в стеке и храните там результаты деления
; Не забудьте перевести цифры в их ASCII коды.
print_uint:
    xor rax, rax            ; 0
    
    mov rax, rdi            ; rax = arg
    mov rsi, DECIMAIL_BASE  ; 10
    mov rcx, 23             ; counter
    sub rsp, 24             ; allocate stack mem
    mov byte [rsp+23], ASCII_NULL_CHAR

.start:
    xor rdx, rdx            ; remainder
    div rsi                 ; rax divide on 10 -> to rax
                            ; remainder saves -> to rdx

    add dl, '0'             ; rdx 1st byte to ASCII
    dec rcx                 ; rcx -= 1
    mov [rsp+rcx], dl       ; dl -> symbol with address rsi

    test rax, rax           ; cmp 0
    jnz .start              ; if not 0
    
    lea rdi, [rsp+rcx]      ; addr of num for out -> rdi
    call print_string
    add rsp, 24
    ret



;=====================================================================================
; Выводит знаковое 8-байтовое число в десятичном формате 
print_int:
    cmp rdi, ASCII_NULL_CHAR    ; arg in rdi
    jns print_uint              ; SignFlag

    neg rdi                     ; * (-1)
    push rdi                    
    mov rdi, "-"                
    call print_char
    pop rdi
    jmp print_uint              



;=====================================================================================
; Принимает два указателя на нуль-терминированные строки, возвращает 1 если они равны, 0 иначе
string_equals:
    ; rdi - 1st ptr
    ; rsi - 2nd ptr

.loop:
    lodsb               ; byte from rsi_ptr to al
    scasb               ; cmp byte in al with rdi_ptr 
    jne .finish         ; if not equals goto finish
    test al, al         ; is al - 0 ?
    jnz .loop           ; if not 0 goto begin

.equal:
    mov rax, 1
    ret

.finish:
    xor rax, rax
    ret
       


;=====================================================================================
; Читает один символ из stdin и возвращает его. Возвращает 0 если достигнут конец потока
read_char:
    push 0                      ; allocate stack for ans
    mov rsi, rsp        
    mov rax, SYS_READ_CODE
    mov rdi, STDIN_CODE
    mov rdx, 1
    syscall
    pop rax
    ret



;=====================================================================================
; Принимает: адрес начала буфера, размер буфера
; Читает в буфер слово из stdin, пропуская пробельные символы в начале, .
; Пробельные символы это пробел 0x20, табуляция 0x9 и перевод строки 0xA.
; Останавливается и возвращает 0 если слово слишком большое для буфера
; При успехе возвращает адрес буфера в rax, длину слова в rdx.
; При неудаче возвращает 0 в rax
; Эта функция должна дописывать к слову нуль-терминатор
read_word:	

    push r12
    push r13
    push r14

    mov r12, rdi    ; адрес начала буфера
    mov r13, rsi    ; размер буфера
    xor r14, r14

.start_whitespace:
    call read_char  ; rax
    
	cmp rax, ASCII_SPACE_CHAR
	je .start_whitespace

	cmp rax, ASCII_TAB_CHAR
	je .start_whitespace

	cmp rax, ASCII_NEW_LINE_CHAR
	je .start_whitespace

	jmp .wait_whitespace

.wait_whitespace:
	cmp rax, ASCII_SPACE_CHAR
	je .write_zero	

	cmp rax, ASCII_TAB_CHAR
	je .write_zero

	cmp rax, ASCII_NEW_LINE_CHAR
	je .write_zero

	test rax, rax   ; eof
	je .write_zero

.write_to_buffer:
    inc r14
    cmp r14, r13     ; buffer size
    ja .overflow
    
    mov [r12 + r14 - 1], al
    test rax, rax   ; eof
    je .success  
    jmp .read_char

.write_zero:
    xor rax, rax
    jmp .write_to_buffer

.read_char:
	call read_char
	jmp .wait_whitespace

.overflow:
	xor rax, rax
	jmp .end

.success:
	mov rax, r12
	lea rdx, [r14 - 1]

.end:
    pop r14
	pop r13
	pop r12
	ret


;=====================================================================================
; Принимает указатель на строку, пытается
; прочитать из её начала беззнаковое число.
; Возвращает в rax: число, rdx : его длину в символах
; rdx = 0 если число прочитать не удалось
parse_uint:
    xor rax, rax
    push r15                            ; save before calee save
    xor r15, r15                     ; counter
    push r12
    xor r12, r12                    ; buffer
    mov r11, DECIMAIL_BASE

.start:
    mov r12b, byte [rdi+r15]         ; curr char in least byte of r12
    sub r12b, '0'                   ; into a numerical value
    jl .end                         ; if < 0
    cmp r12b, 9                     ; cmp to 9
    jg .end                         ; if > 9

    mul r11
    add rax, r12                    ; add curr char to num in rax
    inc r15
    jmp .start

.end:
    mov rdx, r15                    ; return the length in rdx
    pop r12
    pop r15
    ret



;=====================================================================================
; Принимает указатель на строку, пытается
; прочитать из её начала знаковое число.
; Если есть знак, пробелы между ним и числом не разрешены.
; Возвращает в rax: число, rdx : его длину в символах (включая знак, если он был) 
; rdx = 0 если число прочитать не удалось
parse_int:
    xor rax, rax
    cmp byte [rdi], '-'         ; first char cmp
    je .sign                    ; if - goto sign
    cmp byte [rdi], '+'         
    jne parse_uint              ; unsigned

.sign:
    push rdi
    inc rdi                     ; next char
    call parse_uint
    inc rdx                     ; + sign sizeof

    pop rdi
    cmp byte [rdi], '+'
    je .end                     ; if + goto end
    neg rax                     ; * (-1)

.end:
    ret 



;=====================================================================================
; Принимает указатель на строку, указатель на буфер и длину буфера
; Копирует строку в буфер
; Возвращает длину строки если она умещается в буфер, иначе 0
string_copy:
    ; rdi - str_ptr
    ; rsi - buffer_ptr
    ; rdx - buffer_length

    ; returns: rax - str_length | 0 (out of size)
    
	push	r12
	push	r13
	push	r14
    mov     r12, rdi
    mov     r13, rsi
    mov     r14, rdx
    
	call    string_length
	
	cmp   r14, rax
	jl	  .out_of_size

    mov   rcx, rax
    inc   rcx       ; \0
    mov   rsi, r12
    mov   rdi, r13
	rep   movsb
    jmp   .exit
.out_of_size:
	xor	  rax, rax
.exit:
    pop   r14
    pop   r13
    pop   r12
    ret





;=====================================================================================
