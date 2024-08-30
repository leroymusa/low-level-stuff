[org 0x7c00]
bits 16

start:
    cli
    xor ax, ax
    mov ds, ax
    mov es, ax
    mov ss, ax
    mov sp, 0x7c00
    sti

    mov si, msg_loading_kernel
    call print_string

    mov bx, 0x1000
    mov ah, 0x02
    mov al, 1
    mov ch, 0
    mov cl, 2
    mov dh, 0
    mov dl, 0
    int 0x13

    jc disk_read_failed
    
    mov si, msg_disk_read_success
    call print_string

    jmp 0x1000:0000

disk_read_failed:
    mov si, msg_disk_read_failed
    call print_string
    hlt

print_string:
    mov ah, 0x0e
.print_char:
    lodsb
    cmp al, 0
    je .done
    int 0x10
    jmp .print_char
.done:
    ret

msg_loading_kernel db 'Loading kernel...', 0
msg_disk_read_success db 'Kernel loaded successfully.', 0
msg_disk_read_failed db 'Disk read error!', 0

times 510-($-$$) db 0
dw 0xaa55