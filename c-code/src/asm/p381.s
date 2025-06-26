.intel_syntax noprefix

.global field_characteristic
field_characteristic:
	.quad 0xFFFFFFFFFFFFFFFF, 0xFFFFFFFFFFFFFFFF, 0xFFFFFFFFFFFFFFFF, 0xFFFFFFFFFFFFFFFF 
    .quad 0xFFFFFFFFFFFFFFFF, 0x1B4FFFFFFFFFFFFF


.global inverse_minus_field_characteristic
inverse_minus_field_characteristic:
	.quad 0x1 


.text
.p2align 4,,15

.global multiprecision_add_asm
multiprecision_add_asm:
	mov rax, [rsi +  0]
	add rax, [rdx +  0]
	mov [rdi +  0], rax
	.set k, 1
	.rept 5
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

	.rept 5
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
	sbb rsi, [rip + field_characteristic + 8]
	mov rdx, [rbp +  16]
	sbb rdx, [rip + field_characteristic + 16]
	mov rcx, [rbp +  24]
	sbb rcx, [rip + field_characteristic + 24]
	mov r8, [rbp +  32]
	sbb r8, [rip + field_characteristic + 32]
	mov r9, [rbp +  40]
	sbb r9, [rip + field_characteristic + 40]

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
	cswap2 rdx, [rbp +  16]
	cswap2 rcx, [rbp +  24]
	cswap2 r8, [rbp +  32]
	cswap2 r9, [rbp +  40]

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
    test rax, rax
    cmovnz rax, [rip + field_characteristic +  0]
    cmovnz rsi, [rip + field_characteristic +  8]
    cmovnz rdx, [rip + field_characteristic + 16]
    cmovnz rcx, [rip + field_characteristic + 24]
    cmovnz r8,  [rip + field_characteristic + 32]
    cmovnz r9,  [rip + field_characteristic + 40]
    add [rdi +  0], rax
    adc [rdi +  8], rsi
    adc [rdi + 16], rdx
    adc [rdi + 24], rcx
    adc [rdi + 32],  r8
    adc [rdi + 40],  r9
    ret

.global fp_mul
fp_mul:
	push rbp
	push rbx

	sub rsp, 64
	mov [rsp + 56], rdi
	mov rdi,rsi
	mov rsi,rdx

	xor rax,rax
	mov [rsp + 0], rax
	mov [rsp + 8], rax
	mov [rsp + 16], rax
	mov [rsp + 24], rax
	mov [rsp + 32], rax
	mov [rsp + 40], rax
	mov [rsp + 48], rax

.macro MULSTEP, k, I0,I1,I2,I3,I4,I5,I6
	mov r11,[rsp + \I0]
	mov rdx, [rsi +  0]
	mulx rcx, rdx, [rdi + 8*\k]
	add rdx, r11
	mulx rcx, rdx, [rip + inverse_minus_field_characteristic]
	xor rax, rax /* clear flags */

	mulx rbx, rax, [rip + field_characteristic +  0]
	adox r11, rax
	mov [rsp + \I0], r11

	mov r11,[rsp + \I1]
	mulx rcx, rax, [rip + field_characteristic + 8]
	adcx r11, rbx
	adox r11, rax
	mov [rsp + \I1], r11

	mov r11,[rsp + \I2]
	mulx rbx, rax, [rip + field_characteristic + 16]
	adcx r11, rcx
	adox r11, rax
	mov [rsp + \I2], r11

	mov r11,[rsp + \I3]
	mulx rcx, rax, [rip + field_characteristic + 24]
	adcx r11, rbx
	adox r11, rax
	mov [rsp + \I3], r11

	mov r11,[rsp + \I4]
	mulx rbx, rax, [rip + field_characteristic + 32]
	adcx r11, rcx
	adox r11, rax
	mov [rsp + \I4], r11

	mov r11,[rsp + \I5]
	mulx rcx, rax, [rip + field_characteristic + 40]
	adcx r11, rbx
	adox r11, rax
	mov [rsp + \I5], r11

	mov r11,[rsp + \I6]
	mov rax, 0
	adcx r11, rcx
	adox r11, rax
	mov [rsp + \I6], r11

	mov rdx, [rdi + 8*\k]

	xor rax, rax /* clear flags */

	mov r11,[rsp + \I0]
	mulx rbx, rax, [rsi +  0]
	adox r11, rax
	mov [rsp + \I0], r11

	mov r11, [rsp + \I1]
	mulx rcx, rax, [rsi + 8]
	adcx r11, rbx
	adox r11, rax
	mov [rsp + \I1], r11

	mov r11, [rsp + \I2]
	mulx rbx, rax, [rsi + 16]
	adcx r11, rcx
	adox r11, rax
	mov [rsp + \I2], r11

	mov r11, [rsp + \I3]
	mulx rcx, rax, [rsi + 24]
	adcx r11, rbx
	adox r11, rax
	mov [rsp + \I3], r11

	mov r11, [rsp + \I4]
	mulx rbx, rax, [rsi + 32]
	adcx r11, rcx
	adox r11, rax
	mov [rsp + \I4], r11

	mov r11, [rsp + \I5]
	mulx rcx, rax, [rsi + 40]
	adcx r11, rbx
	adox r11, rax
	mov [rsp + \I5], r11

	mov r11, [rsp + \I6]
	mov rax, 0
	adcx r11, rcx
	adox r11, rax
	mov [rsp + \I6], r11

.endm

MULSTEP 0,8,16,24,32,40,48,0
MULSTEP 1,16,24,32,40,48,0,8
MULSTEP 2,24,32,40,48,0,8,16
MULSTEP 3,32,40,48,0,8,16,24
MULSTEP 4,40,48,0,8,16,24,32
MULSTEP 5,48,0,8,16,24,32,40

	mov rdi, [rsp + 56]

	mov r11, [rsp + 0]
	mov [rdi + 0], r11
	mov r11, [rsp + 8]
	mov [rdi + 8], r11
	mov r11, [rsp + 16]
	mov [rdi + 16], r11
	mov r11, [rsp + 24]
	mov [rdi + 24], r11
	mov r11, [rsp + 32]
	mov [rdi + 32], r11
	mov r11, [rsp + 40]
	mov [rdi + 40], r11
	add rsp, 64

	pop rbx
	pop rbp
	jmp .reduce_once

.global fp_sqr
fp_sqr:
	mov rdx, rsi
	jmp fp_mul
