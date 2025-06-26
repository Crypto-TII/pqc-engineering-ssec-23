.intel_syntax noprefix

.global field_characteristic
field_characteristic:
	.quad 0xFFFFFFFFFFFFFFFF, 0xFFFFFFFFFFFFFFFF, 0xFFFFFFFFFFFFFFFF, 0xFFFFFFFFFFFFFFFF 
	.quad 0xFFFFFFFFFFFFFFFF, 0xFFFFFFFFFFFFFFFF, 0xFFFFFFFFFFFFFFFF, 0xFFFFFFFFFFFFFFFF 
	.quad 0xFFFFFFFFFFFFFFFF, 0xFFFFFFFFFFFFFFFF, 0xFFFFFFFFFFFFFFFF, 0xFFFFFFFFFFFFFFFF
    .quad 0x66BF

.global inverse_minus_field_characteristic
inverse_minus_field_characteristic:
	.quad 0x1 

.text
.p2align 4,,15

.global multiprecision_addition_asm
multiprecision_addition_asm:
	mov rax, [rsi +  0]
	add rax, [rdx +  0]
	mov [rdi +  0], rax
	.set k, 1
	.rept 12
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

	.rept 12
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
    sub rsp, 40
    mov rbp, rdi

    mov rdi, [rbp + 0]
    sub rdi, [rip + field_characteristic + 0]
    movq [rsp + 0], rdi
    mov rsi, [rbp + 8]
    sbb rsi, [rip + field_characteristic + 8]
    movq [rsp + 8], rsi
    mov rdx, [rbp + 16]
    sbb rdx, [rip + field_characteristic + 16]
    movq [rsp + 16], rdx
    mov rcx, [rbp + 24]
    sbb rcx, [rip + field_characteristic + 24]
    movq [rsp + 24], rcx
    mov r8,  [rbp + 32]
    sbb r8,  [rip + field_characteristic + 32]
    movq [rsp + 32], r8
    mov r9,  [rbp + 40]
    sbb r9,  [rip + field_characteristic + 40]
    mov r10, [rbp + 48]
    sbb r10, [rip + field_characteristic + 48]
    mov r11, [rbp + 56]
    sbb r11, [rip + field_characteristic + 56]
    mov rdi, [rbp + 64]
    sbb rdi, [rip + field_characteristic + 64]
    mov rsi, [rbp + 72]
    sbb rsi, [rip + field_characteristic + 72]
    mov rdx, [rbp + 80]
    sbb rdx, [rip + field_characteristic + 80]
    mov rcx, [rbp + 88]
    sbb rcx, [rip + field_characteristic + 88]
    mov r8,  [rbp + 96]
    sbb r8,  [rip + field_characteristic + 96]

    setnc al
    movzx rax, al
    neg rax

.macro cswap2, r, m
    xor \r, \m
    and \r, rax
    xor \m, \r
.endm

    cswap2 r9, [rbp + 40]
    cswap2 r10, [rbp + 48]
    cswap2 r11, [rbp + 56]
    cswap2 rdi, [rbp + 64]
    cswap2 rsi, [rbp + 72]
    cswap2 rdx, [rbp + 80]
    cswap2 rcx, [rbp + 88]
    cswap2 r8, [rbp + 96]
    movq rdi, [rsp + 0]
    cswap2 rdi, [rbp + 0]
    movq rsi, [rsp + 8]
    cswap2 rsi, [rbp + 8]
    movq rdx, [rsp + 16]
    cswap2 rdx, [rbp + 16]
    movq rcx, [rsp + 24]
    cswap2 rcx, [rbp + 24]
    movq r8, [rsp + 32]
    cswap2 r8, [rbp + 32]

    add rsp, 40
    pop rbp
    ret

.global fp_add
fp_add:
    push rdi
    call multiprecision_addition_asm
    pop rdi
    jmp .reduce_once

.global fp_sub
fp_sub:
    push rdi
    call multiprecision_subtract_asm
    pop rdi
    
    push r12
    push r13
    push r14
    push r15
    push rbp
    xor rsi, rsi
    xor rdx, rdx
    xor rcx, rcx
    xor r8, r8
    xor r9, r9
    xor r10, r10
    xor r11, r11
    xor r12, r12
    xor r13, r13
    xor r14, r14
    xor r15, r15
    xor rbp, rbp
    test rax, rax
    cmovnz rax, [rip + field_characteristic +  0]
    cmovnz rsi, [rip + field_characteristic +  8]
    cmovnz rdx, [rip + field_characteristic + 16]
    cmovnz rcx, [rip + field_characteristic + 24]
    cmovnz r8,  [rip + field_characteristic + 32]
    cmovnz r9,  [rip + field_characteristic + 40]
    cmovnz r10, [rip + field_characteristic + 48]
    cmovnz r11, [rip + field_characteristic + 56]
    cmovnz r12, [rip + field_characteristic + 64]
    cmovnz r13, [rip + field_characteristic + 72]
    cmovnz r14, [rip + field_characteristic + 80]
    cmovnz r15, [rip + field_characteristic + 88]
    cmovnz rbp, [rip + field_characteristic + 96]
    add [rdi +  0], rax
    adc [rdi +  8], rsi
    adc [rdi + 16], rdx
    adc [rdi + 24], rcx
    adc [rdi + 32],  r8
    adc [rdi + 40],  r9
    adc [rdi + 48], r10
    adc [rdi + 56], r11
    adc [rdi + 64], r12
    adc [rdi + 72], r13
    adc [rdi + 80], r14
    adc [rdi + 88], r15
    adc [rdi + 96], rbp

    pop rbp
    pop r15
    pop r14
    pop r13
    pop r12
    ret


.global fp_mul
fp_mul:
    push rbp
    push rbx

    sub rsp,120
    mov [rsp+112],rdi
    mov rdi,rsi
    mov rsi,rdx

    /* XXX: put directly into output */
    xor rax,rax
    mov [rsp+0],rax
    mov [rsp+8],rax
    mov [rsp+16],rax
    mov [rsp+24],rax
    mov [rsp+32],rax
    mov [rsp+40],rax
    mov [rsp+48],rax
    mov [rsp+56],rax
    mov [rsp+64],rax
    mov [rsp+72],rax
    mov [rsp+80],rax
    mov [rsp+88],rax
    mov [rsp+96],rax
    mov [rsp+104],rax

.macro MULSTEP, k, I0, I1, I2, I3, I4, I5, I6, I7, I8, I9, I10, I11, I12, I13

    mov r11,[rsp+\I0]
    mov rdx, [rsi +  0]
    mulx rcx, rdx, [rdi + 8*\k]
    add rdx, r11
    mulx rcx, rdx, [rip + inverse_minus_field_characteristic]

    xor rax, rax /* clear flags */

    mulx rbx, rax, [rip + field_characteristic +  0]
    adox r11, rax
    mov [rsp+\I0],r11

    mov r11,[rsp+\I1]
    mulx rcx, rax, [rip + field_characteristic + 8]
    adcx r11, rbx
    adox r11, rax
    mov [rsp+\I1],r11

    mov r11,[rsp+\I2]
    mulx rbx, rax, [rip + field_characteristic + 16]
    adcx r11, rcx
    adox r11, rax
    mov [rsp+\I2],r11

    mov r11,[rsp+\I3]
    mulx rcx, rax, [rip + field_characteristic + 24]
    adcx r11, rbx
    adox r11, rax
    mov [rsp+\I3],r11

    mov r11,[rsp+\I4]
    mulx rbx, rax, [rip + field_characteristic + 32]
    adcx r11, rcx
    adox r11, rax
    mov [rsp+\I4],r11

    mov r11,[rsp+\I5]
    mulx rcx, rax, [rip + field_characteristic + 40]
    adcx r11, rbx
    adox r11, rax
    mov [rsp+\I5],r11

    mov r11,[rsp+\I6]
    mulx rbx, rax, [rip + field_characteristic + 48]
    adcx r11, rcx
    adox r11, rax
    mov [rsp+\I6],r11

    mov r11,[rsp+\I7]
    mulx rcx, rax, [rip + field_characteristic + 56]
    adcx r11, rbx
    adox r11, rax
    mov [rsp+\I7],r11

    mov r11,[rsp+\I8]
    mulx rbx, rax, [rip + field_characteristic + 64]
    adcx r11, rcx
    adox r11, rax
    mov [rsp+\I8],r11

    mov r11,[rsp+\I9]
    mulx rcx, rax, [rip + field_characteristic + 72]
    adcx r11, rbx
    adox r11, rax
    mov [rsp+\I9],r11

    mov r11,[rsp+\I10]
    mulx rbx, rax, [rip + field_characteristic + 80]
    adcx r11, rcx
    adox r11, rax
    mov [rsp+\I10],r11

    mov r11,[rsp+\I11]
    mulx rcx, rax, [rip + field_characteristic + 88]
    adcx r11, rbx
    adox r11, rax
    mov [rsp+\I11],r11

    mov r11,[rsp+\I12]
    mulx rbx, rax, [rip + field_characteristic + 96]
    adcx r11, rcx
    adox r11, rax
    mov [rsp+\I12],r11

    mov r11,[rsp+\I13]
    mov rax, 0
    adcx r11, rbx
    adox r11, rax
    mov [rsp+\I13],r11

    mov rdx, [rdi + 8*\k]

    xor rax, rax /* clear flags */

    mov r11,[rsp+\I0]
    mulx rbx, rax, [rsi +  0]
    adox r11, rax
    mov [rsp+\I0],r11

    mov r11,[rsp+\I1]
    mulx rcx, rax, [rsi + 8]
    adcx r11, rbx
    adox r11, rax
    mov [rsp+\I1],r11

    mov r11,[rsp+\I2]
    mulx rbx, rax, [rsi + 16]
    adcx r11, rcx
    adox r11, rax
    mov [rsp+\I2],r11

    mov r11,[rsp+\I3]
    mulx rcx, rax, [rsi + 24]
    adcx r11, rbx
    adox r11, rax
    mov [rsp+\I3],r11

    mov r11,[rsp+\I4]
    mulx rbx, rax, [rsi + 32]
    adcx r11, rcx
    adox r11, rax
    mov [rsp+\I4],r11

    mov r11,[rsp+\I5]
    mulx rcx, rax, [rsi + 40]
    adcx r11, rbx
    adox r11, rax
    mov [rsp+\I5],r11

    mov r11,[rsp+\I6]
    mulx rbx, rax, [rsi + 48]
    adcx r11, rcx
    adox r11, rax
    mov [rsp+\I6],r11

    mov r11,[rsp+\I7]
    mulx rcx, rax, [rsi + 56]
    adcx r11, rbx
    adox r11, rax
    mov [rsp+\I7],r11

    mov r11,[rsp+\I8]
    mulx rbx, rax, [rsi + 64]
    adcx r11, rcx
    adox r11, rax
    mov [rsp+\I8],r11

    mov r11,[rsp+\I9]
    mulx rcx, rax, [rsi + 72]
    adcx r11, rbx
    adox r11, rax
    mov [rsp+\I9],r11

    mov r11,[rsp+\I10]
    mulx rbx, rax, [rsi + 80]
    adcx r11, rcx
    adox r11, rax
    mov [rsp+\I10],r11

    mov r11,[rsp+\I11]
    mulx rcx, rax, [rsi + 88]
    adcx r11, rbx
    adox r11, rax
    mov [rsp+\I11],r11

    mov r11,[rsp+\I12]
    mulx rbx, rax, [rsi + 96]
    adcx r11, rcx
    adox r11, rax
    mov [rsp+\I12],r11

    mov r11,[rsp+\I13]
    mov rax, 0
    adcx r11, rbx
    adox r11, rax
    mov [rsp+\I13],r11

.endm

    MULSTEP 0, 8, 16, 24, 32, 40, 48, 56, 64, 72, 80, 88, 96, 104, 0
    MULSTEP 1, 16, 24, 32, 40, 48, 56, 64, 72, 80, 88, 96, 104, 0, 8
    MULSTEP 2, 24, 32, 40, 48, 56, 64, 72, 80, 88, 96, 104, 0, 8, 16
    MULSTEP 3, 32, 40, 48, 56, 64, 72, 80, 88, 96, 104, 0, 8, 16, 24
    MULSTEP 4, 40, 48, 56, 64, 72, 80, 88, 96, 104, 0, 8, 16, 24, 32
    MULSTEP 5, 48, 56, 64, 72, 80, 88, 96, 104, 0, 8, 16, 24, 32, 40
    MULSTEP 6, 56, 64, 72, 80, 88, 96, 104, 0, 8, 16, 24, 32, 40, 48
    MULSTEP 7, 64, 72, 80, 88, 96, 104, 0, 8, 16, 24, 32, 40, 48, 56
    MULSTEP 8, 72, 80, 88, 96, 104, 0, 8, 16, 24, 32, 40, 48, 56, 64
    MULSTEP 9, 80, 88, 96, 104, 0, 8, 16, 24, 32, 40, 48, 56, 64, 72
    MULSTEP 10, 88, 96, 104, 0, 8, 16, 24, 32, 40, 48, 56, 64, 72, 80
    MULSTEP 11, 96, 104, 0, 8, 16, 24, 32, 40, 48, 56, 64, 72, 80, 88
    MULSTEP 12, 104, 0, 8, 16, 24, 32, 40, 48, 56, 64, 72, 80, 88, 96

    mov rdi,[rsp+112]

    mov r11,[rsp+0]
    mov [rdi+0],r11
    mov r11,[rsp+8]
    mov [rdi+8],r11
    mov r11,[rsp+16]
    mov [rdi+16],r11
    mov r11,[rsp+24]
    mov [rdi+24],r11
    mov r11,[rsp+32]
    mov [rdi+32],r11
    mov r11,[rsp+40]
    mov [rdi+40],r11
    mov r11,[rsp+48]
    mov [rdi+48],r11
    mov r11,[rsp+56]
    mov [rdi+56],r11
    mov r11,[rsp+64]
    mov [rdi+64],r11
    mov r11,[rsp+72]
    mov [rdi+72],r11
    mov r11,[rsp+80]
    mov [rdi+80],r11
    mov r11,[rsp+88]
    mov [rdi+88],r11
    mov r11,[rsp+96]
    mov [rdi+96],r11

    add rsp,120

    pop rbx
    pop rbp
    jmp .reduce_once

.global fp_sqr
fp_sqr:
	mov rdx, rsi
	jmp fp_mul
