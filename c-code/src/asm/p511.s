.intel_syntax noprefix

.global field_characteristic
field_characteristic:
    .quad 0x1b81b90533c6c87b, 0xc2721bf457aca835, 0x516730cc1f0b4f25, 0xa7aac6c567f35507
    .quad 0x5afbfcc69322c9cd, 0xb42d083aedc88c42, 0xfc8ab0d15e3e4c4a, 0x65b48e8f740f89bf

.global inverse_minus_field_characteristic
inverse_minus_field_characteristic:
    .quad 0x66c1301f632e294d

.global multiprecision_add_asm
multiprecision_add_asm:
    mov rax, [rsi +  0]
    add rax, [rdx +  0]
    mov [rdi +  0], rax
    .set k, 1
    .rept 7
        mov rax, [rsi + 8*k]
        adc rax, [rdx + 8*k]
        mov [rdi + 8*k], rax
        .set k, k+1
    .endr
    setc al
    movzx rax, al
    ret

.global multiprecision_subtract_asm
multiprecision_subtract_asm:
    mov rax, [rsi +  0]
    sub rax, [rdx +  0]
    mov [rdi +  0], rax
    .set k, 1
    .rept 7
        mov rax, [rsi + 8*k]
        sbb rax, [rdx + 8*k]
        mov [rdi + 8*k], rax
        .set k, k+1
    .endr
    setc al
    movzx rax, al
    ret

.reduce_once:
    push rbp
    mov rbp, rdi

    mov rdi, [rbp +  0]
    sub rdi, [rip + field_characteristic +  0]
    mov rsi, [rbp +  8]
    sbb rsi, [rip + field_characteristic +  8]
    mov rdx, [rbp + 16]
    sbb rdx, [rip + field_characteristic + 16]
    mov rcx, [rbp + 24]
    sbb rcx, [rip + field_characteristic + 24]
    mov r8,  [rbp + 32]
    sbb r8,  [rip + field_characteristic + 32]
    mov r9,  [rbp + 40]
    sbb r9,  [rip + field_characteristic + 40]
    mov r10, [rbp + 48]
    sbb r10, [rip + field_characteristic + 48]
    mov r11, [rbp + 56]
    sbb r11, [rip + field_characteristic + 56]

    setnc al
    movzx rax, al
    neg rax

.macro cswap2, r, m
    xor \r, \m
    and \r, rax
    xor \m, \r
.endm

    cswap2 rdi, [rbp +  0]
    cswap2 rsi, [rbp +  8]
    cswap2 rdx, [rbp + 16]
    cswap2 rcx, [rbp + 24]
    cswap2 r8,  [rbp + 32]
    cswap2 r9,  [rbp + 40]
    cswap2 r10, [rbp + 48]
    cswap2 r11, [rbp + 56]

    pop rbp
    ret

.global fp_add
fp_add:
    push rdi
    call multiprecision_add_asm
    pop rdi
    jmp .reduce_once

.global fp_sub
fp_sub:
    push rdi
    call multiprecision_subtract_asm
    pop rdi
    xor rsi, rsi
    xor rdx, rdx
    xor rcx, rcx
    xor r8, r8
    xor r9, r9
    xor r10, r10
    xor r11, r11
    test rax, rax
    cmovnz rax, [rip + field_characteristic +  0]
    cmovnz rsi, [rip + field_characteristic +  8]
    cmovnz rdx, [rip + field_characteristic + 16]
    cmovnz rcx, [rip + field_characteristic + 24]
    cmovnz r8,  [rip + field_characteristic + 32]
    cmovnz r9,  [rip + field_characteristic + 40]
    cmovnz r10, [rip + field_characteristic + 48]
    cmovnz r11, [rip + field_characteristic + 56]
    add [rdi +  0], rax
    adc [rdi +  8], rsi
    adc [rdi + 16], rdx
    adc [rdi + 24], rcx
    adc [rdi + 32],  r8
    adc [rdi + 40],  r9
    adc [rdi + 48], r10
    adc [rdi + 56], r11
    ret

.global fp_mul
fp_mul:
    push rbp
    push rbx
    push r12
    push r13
    push r14
    push r15

    push rdi

    mov rdi, rsi
    mov rsi, rdx

    xor r8,  r8
    xor r9,  r9
    xor r10, r10
    xor r11, r11
    xor r12, r12
    xor r13, r13
    xor r14, r14
    xor r15, r15
    xor rbp, rbp

    /* flags are already cleared */

.macro MULSTEP, k, r0, r1, r2, r3, r4, r5, r6, r7, r8

    mov rdx, [rsi +  0]
    mulx rcx, rdx, [rdi + 8*\k]
    add rdx, \r0
    mulx rcx, rdx, [rip + inverse_minus_field_characteristic]

    xor rax, rax /* clear flags */

    mulx rbx, rax, [rip + field_characteristic +  0]
    adox \r0, rax

    mulx rcx, rax, [rip + field_characteristic +  8]
    adcx \r1, rbx
    adox \r1, rax

    mulx rbx, rax, [rip + field_characteristic + 16]
    adcx \r2, rcx
    adox \r2, rax

    mulx rcx, rax, [rip + field_characteristic + 24]
    adcx \r3, rbx
    adox \r3, rax

    mulx rbx, rax, [rip + field_characteristic + 32]
    adcx \r4, rcx
    adox \r4, rax

    mulx rcx, rax, [rip + field_characteristic + 40]
    adcx \r5, rbx
    adox \r5, rax

    mulx rbx, rax, [rip + field_characteristic + 48]
    adcx \r6, rcx
    adox \r6, rax

    mulx rcx, rax, [rip + field_characteristic + 56]
    adcx \r7, rbx
    adox \r7, rax

    mov rax, 0
    adcx \r8, rcx
    adox \r8, rax


    mov rdx, [rdi + 8*\k]

    xor rax, rax /* clear flags */

    mulx rbx, rax, [rsi +  0]
    adox \r0, rax

    mulx rcx, rax, [rsi +  8]
    adcx \r1, rbx
    adox \r1, rax

    mulx rbx, rax, [rsi + 16]
    adcx \r2, rcx
    adox \r2, rax

    mulx rcx, rax, [rsi + 24]
    adcx \r3, rbx
    adox \r3, rax

    mulx rbx, rax, [rsi + 32]
    adcx \r4, rcx
    adox \r4, rax

    mulx rcx, rax, [rsi + 40]
    adcx \r5, rbx
    adox \r5, rax

    mulx rbx, rax, [rsi + 48]
    adcx \r6, rcx
    adox \r6, rax

    mulx rcx, rax, [rsi + 56]
    adcx \r7, rbx
    adox \r7, rax

    mov rax, 0
    adcx \r8, rcx
    adox \r8, rax

.endm

    MULSTEP 0, r8,  r9,  r10, r11, r12, r13, r14, r15, rbp
    MULSTEP 1, r9,  r10, r11, r12, r13, r14, r15, rbp, r8
    MULSTEP 2, r10, r11, r12, r13, r14, r15, rbp, r8,  r9
    MULSTEP 3, r11, r12, r13, r14, r15, rbp, r8,  r9,  r10
    MULSTEP 4, r12, r13, r14, r15, rbp, r8,  r9,  r10, r11
    MULSTEP 5, r13, r14, r15, rbp, r8,  r9,  r10, r11, r12
    MULSTEP 6, r14, r15, rbp, r8,  r9,  r10, r11, r12, r13
    MULSTEP 7, r15, rbp, r8,  r9,  r10, r11, r12, r13, r14

    pop rdi

    mov [rdi +  0], rbp
    mov [rdi +  8], r8
    mov [rdi + 16], r9
    mov [rdi + 24], r10
    mov [rdi + 32], r11
    mov [rdi + 40], r12
    mov [rdi + 48], r13
    mov [rdi + 56], r14

    pop r15
    pop r14
    pop r13
    pop r12
    pop rbx
    pop rbp
    jmp .reduce_once

.global fp_sqr
fp_sqr:
    mov rdx, rsi
    jmp fp_mul
