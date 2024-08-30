section .data
    array db 3, 1, 4, 1, 5, 9, 2, 6, 5, 3
    length equ $ - array
    max_msg db 'Max: ', 0
    newline db 10, 0

section .bss
    max_value resb 1
    digit resb 1

section .text
    global _start

_start:
    mov rsi, array
    mov al, [rsi]
    mov [max_value], al

    mov rcx, length
    dec rcx

find_max:
    inc rsi
    mov al, [rsi]
    cmp al, [max_value]
    jbe skip
    mov [max_value], al

skip:
    loop find_max

    mov rax, 1
    mov rdi, 1
    mov rsi, max_msg
    mov rdx, 5
    syscall

    mov al, [max_value]
    add al, '0'
    mov [digit], al

    mov rax, 1
    mov rdi, 1
    mov rsi, digit
    mov rdx, 1
    syscall

    mov rax, 1
    mov rdi, 1
    mov rsi, newline
    mov rdx, 1
    syscall

    mov rax, 60
    xor rdi, rdi
    syscall