#!/usr/bin/env sage -python


# how to call
# sage -python AsmMultCodegenerator.py  > fp9216.s

from sage.all import *
import math

#///////////////////////////////////////////////////
#//p word extractor [repeat pword times]
def WordExtractor(p, pwords, Var):

    k = 64
    PWord = []
    S =  ".global " + Var + "\n" + Var + ":\n"

    pp = p

    for i in range(1,pwords+1):
        if (((i - 1)%4) == 0):
            S = S + ".quad "
        #PWord[i] = pp%2**k
        PWord.append(pp%2**k)
        if((i%4) != 0):
            #S = S + str(hex(PWord[i-1])).upper() + ","
            S = S + "0x{:X}".format(PWord[i-1]) +  ","
        else:
            #S = S + str(hex(PWord[i-1])).upper() + " \n"
            S = S + "0x{:X}".format(PWord[i-1]) +  " \n"
        pp = pp // (2**k)

    S =  S + "\n\n"
    
    print(S)
    return S


#//////////////////////////////////////////////////////////////////
def PrintHeader(pbits, pbytes, plimbs):

    S = ".intel_syntax noprefix\n\n"
    S = S + ".section .rodata\n\n"
    S = S + ".set pbits," + str(pbits) + "\n"
    S = S + ".set pbytes," + str(pbytes) + "\n"
    S = S + ".set plimbs," + str(plimbs) + "\n\n"
    print(S)
    return S

#//////////////////////////////////////////////////////////////////
def PrintUintHeader(plimbs):

    S = ".intel_syntax noprefix\n\n"
    S = S + ".section .rodata\n\n"
    S = S + ".global uintbig_1\nuintbig_1:\n"
    S = S + "    .quad 1, 0, 0, 0\n"

    N = (plimbs//4) -1;
    for i in range(1, N+1):
        S = S + "    .quad 0, 0, 0, 0\n"

    N = plimbs%4; 
    if (N != 0):
        S = S + "    .quad"
        for i in range(1,N):
                S = S + " 0,"
        S = S + "0\n"

    S = S + ".section .text\n\n"

    S = S + ".global uintbig_add\nuintbig_add:\n    mov rax, [rsi +  0]\n    add rax, [rdx +  0]\n    mov [rdi +  0], rax\n    .set k, 1\n"

    S = S + "    .rept " + str(plimbs-1) + "\n"

    S = S + "mov rax, [rsi + 8*k]\n        adc rax, [rdx + 8*k]\n        mov [rdi + 8*k], rax\n        .set k, k+1\n    .endr\n    setc al\n    movzx rax, al\n    ret\n\n"

    S = S + ".global uintbig_sub\nuintbig_sub:\n    mov rax, [rsi +  0]\n    sub rax, [rdx +  0]\n    mov [rdi +  0], rax\n    .set k, 1\n"

    S = S + "    .rept " + str(plimbs-1) + "\n"

    S = S + "      mov rax, [rsi + 8*k]\n        sbb rax, [rdx + 8*k]\n        mov [rdi + 8*k], rax\n        .set k, k+1\n    .endr\n    setc al\n    movzx rax, al\n    ret"

    print(S)
    return 0

#////////////////////////////////////////
def PrintFooter():

    S = ".section .data\n\n"
    S = S + ".global fpadd\nfpadd:\n     .quad 0\n\n"
    S = S + ".global fpsqr\nfpsqr:\n     .quad 0\n\n"
    S = S + ".global fpmul\nfpmul:\n     .quad 0\n\n"
    S = S + ".section .text\n\n.p2align 4,,15\n\n"
    S = S + ".global init_counters\ninit_counters:\n      movq [rip + fpadd], 0\n      movq [rip + fpsqr], 0\n      movq [rip + fpmul], 0\n      ret\n\n"
    S = S + ".global fp_copy\nfp_copy:\n    cld\n    mov rcx, plimbs\n    rep movsq\n    ret\n\n"
    S = S + ".global fp_cswap\nfp_cswap:\n    movzx rax, dl\n    neg rax\n    .set k, 0\n    .rept plimbs\n        mov rcx, [rdi + 8*k]\n"
    S = S + "mov rdx, [rsi + 8*k]\n        mov r8, rcx\n        xor r8, rdx\n        and r8, rax\n\n        xor rcx, r8\n        xor rdx, r8\n\n"
    S = S + "        mov [rdi + 8*k], rcx\n        mov [rsi + 8*k], rdx\n\n        .set k, k+1\n    .endr\n    ret\n\n"

    print(S)
    return S

#////////////////////////////////////////
def PrintRedOnce(pbytes):

    Reg_Ar = ["rdi", "rsi", "rdx", "rcx", "r8", "r9", "r10", "r11"]

    S = ".reduce_once:\n    push rbp\n    mov rbp, rdi\n\n    mov rdi, [rbp +  0]\n    sub rdi, [rip + p +  0]\n"

    N = pbytes-8

    for i in range(8,N+1,8):
        S = S + "    mov " + Reg_Ar[(int(i/8) % 8)] + ", [rbp +  " + str(i) + "]\n    sbb " +  Reg_Ar[(int(i/8) % 8)] + ", [rip + p +  " + str(i) + "]\n"
        if ((int((i+8)/8)%8) == 0) and (i != 0):
            S = S + "\n"

    S = S + "\n    setnc al\n    movzx rax, al\n    neg rax\n\n"
    S = S + ".macro cswap2, r, m\n    xor \\r, \\m\n    and \\r, rax\n    xor \\m, \\r\n.endm\n\n\n"

    n = -(int(pbytes/8) % 8) +8

    for i in range(1, n+1):
        S = S + "    cswap2 " + Reg_Ar[i-1] + ", [rbp +  " + str((pbytes-n*8)+(i-1)*8) + "]\n" 


    N = N - (n*8)
    M = math.ceil(N/64)
    for j in range(1, M+1):

        S = S + "\n    mov rdi, [rbp +  0]\n    sub   rdi, [rip + p +  0]\n"
        for i in range(8,N+1,8):        
            S = S + "    mov " + Reg_Ar[int(i/8) % 8] + ", [rbp +  " + str(i) + "]\n    sbb " + Reg_Ar[int(i/8) % 8] + ", [rip + p +  " + str(i) + "]\n"
            if ((int((i+8)/8)%8) == 0) and (i != 0):
                S = S + "\n"
        for i in range(1,9):
            S = S + "    cswap2 " + Reg_Ar[i-1] + ", [rbp +  " + str((N-64)+i*8) + "]\n"
        N = N - 64

    S = S + "    pop rbp\n    ret\n\n"

    print(S)
    return S

#////////////////////////////////////////
def PrintSub():

    S= ".global fp_add2\nfp_add2:\n  mov rdx, rdi\n\n"
    S = S + ".global fp_add\nfp_add:\n  push rdi\n  call uintbig_add\n  pop rdi\n\n  incq [rip + fpadd]\n\n  jmp .reduce_once\n\n"
    S = S + ".global fp_sub2\nfp_sub2:\n  mov rdx, rdi\n  xchg rsi, rdx\n\n.global fp_sub\nfp_sub:\n  push rdi\n  call uintbig_sub\n  pop rdi\n\n\n"
    S = S + "  incq [rip + fpadd]  /* increasing number of additions performed */\n\n  neg rax\n\n  sub rsp, pbytes\n\n"
    S = S + "  mov rcx, [rip + p +  0]\n  and rcx, rax\n  mov [rsp + 0],rcx\n  .set k, 1\n  .rept plimbs-1\n      mov rcx, [rip + p + 8*k]\n      and rcx, rax\n"
    S = S + "      mov [rsp + 8*k], rcx\n      .set k, k+1\n  .endr\n\n"
    S = S + "  mov rcx, [rsp +  0]\n  add rcx, [rdi +  0]\n  mov [rdi +  0], rcx\n  .set k, 1\n  .rept plimbs-1\n      mov rcx, [rsp + 8*k]\n"
    S = S + "      adc rcx, [rdi + 8*k]\n      mov [rdi + 8*k], rcx\n      .set k, k+1\n  .endr\n\n  add rsp, pbytes\n  ret\n\n\n"
    S = S + "/* Montgomery arithmetic */\n\n.global fp_enc\nfp_enc:\n    lea rdx, [rip + r_squared_mod_p]\n    jmp fp_mul\n\n"
    S = S + ".global fp_dec\nfp_dec:\n    lea rdx, [rip + uintbig_1]\n    jmp fp_mul\n\n\n"

    print(S)
    return S

#////////////////////////////////////////
def PrintMul(pbytes, pwords):

    Reg_Ar = ["rbx", "rcx"]

    S = ".global fp_mul2\nfp_mul2:\n  mov rdx, rdi\n.global fp_mul\nfp_mul:\n  push rbp\n  push rbx\n\n"
    S = S +"  incq [rip + fpmul]  /* increasing number of multiplications performed */\n\n"

    S = S + "  sub rsp, " + str(pbytes + 16) + "\n  mov [rsp+ " + str(pbytes+8) + "],rdi\n  mov rdi,rsi\n  mov rsi,rdx\n\n\n"
    S = S + "  xor rax,rax\n"

    N = pbytes

    for i in range(0,N+1,8):
        S = S + "  mov [rsp+" + str(i) + "],rax\n"



    S = S + "\n\n.macro MULSTEP, k, "
    R = ""
    for i in range(0, pwords):
        R = ""
        R = R + "I" + str(i) + ","
        S = S + R

    S = S + "I" + str(pwords) + "\n\n"
    S = S + "    mov r11,[rsp+\\I0]\n    mov rdx, [rsi +  0]\n    mulx rcx, rdx, [rdi + 8*\\k]\n    add rdx, r11\n    mulx rcx, rdx, [rip + inv_min_p_mod_r]"
    S = S + "\n\n    xor rax, rax /* clear flags */\n\n\n"

    S = S + "    mulx rbx, rax, [rip + p +  0]\n    adox r11, rax\n    mov [rsp+\\I0], r11\n\n"

    N = pbytes-8

    for i in range(8,N+1,8):
        S = S + "    mov r11,[rsp+\\I" + str(int(i/8)) + "]\n    mulx " + Reg_Ar[(int(i/8) % 2)] + ", rax, [rip + p +  " + str(i) + "]\n "
        S = S + "   adcx r11, " + Reg_Ar[-(int(i/8) % 2) +1] + "\n   adox r11, rax\n    mov [rsp+\\I" + str(int(i/8)) + "],r11\n\n"

    S = S + "\n    mov r11,[rsp+\\I" + str(pwords) + "]\n    mov rax, 0\n    adcx r11, rcx\n    adox r11, rax\n    mov [rsp+\\I" + str(pwords) + "],r11\n\n"
    S = S + "    mov rdx, [rdi + 8*\\k]\n\n    xor rax, rax /* clear flags */\n\n"

    S = S + "    mov r11,[rsp+\\I0]\n    mulx rbx, rax, [rsi +  0]\n    adox r11, rax\n    mov [rsp+\\I0],r11\n\n"

    for i in range(8,N+1,8):
        S = S + "    mov r11,[rsp+\\I" + str(int(i/8)) + "]\n    mulx " + Reg_Ar[int(i/8) % 2] + ", rax, [rsi +  " + str(i) + "]\n"
        S = S + "    adcx r11, " + Reg_Ar[-(int(i/8) % 2) +1] + "\n    adox r11, rax\n    mov [rsp+\\I" + str(int(i/8)) + "],r11\n\n"

    S = S + "    mov r11,[rsp+\\I" + str(pwords) + "]\n    mov rax, 0\n    adcx r11, rcx\n    adox r11, rax\n    mov [rsp+\\I" + str(pwords) + "],r11\n\n.endm\n\n"

    T = ""
    for i in range(0, pwords):
        T = T + "MULSTEP " + str(i) + ","
        for j in range(8,pbytes+1,8):
            T = T + str((j + i*8) % (pbytes + 8)) + ","
        S = S + T
        T = ""
        S = S + str((pbytes +8 + i*8) % (pbytes + 8)) + "\n"
    
    S = S + "\n\n    mov rdi,[rsp+" + str(pbytes+8) + "]\n\n"

    for i in range(0,N+1,8):
        S = S + "    mov r11,[rsp+" + str(i) + "]\n    mov [rdi+" + str(i) + "],r11\n"


    S = S + "    add rsp," + str(pbytes+16) + "\n\n    pop rbx\n    pop rbp\n\n    jmp .reduce_once\n\n\n"
    S = S + ".global fp_sq1\nfp_sq1:\n    mov rsi, rdi\n.global fp_sqr\nfp_sqr:\n    mov rdx, rsi\n\n    decq [rip + fpmul]\n    incq [rip + fpsqr]\n\n    jmp fp_mul\n"


    print(S)
    return S


#////////////////////////////////////////
def PrintPow(pwords):

    S = ".global fp_pow\nfp_pow:\n    push rbx\n    mov rbx, rsi\n    push r12\n    push r13\n    push rdi\n    sub rsp, pbytes\n\n"
    S = S + "    mov rsi, rdi\n    mov rdi, rsp\n    call fp_copy\n\n    mov rdi, [rsp + pbytes]\n    lea rsi, [rip + fp_1]\n    call fp_copy\n\n"
    S = S + ".macro POWSTEP, k\n        mov r13, [rbx + 8*\\k]\n        xor r12, r12\n\n"
    S = S + "        0:\n        test r13, 1\n        jz 1f\n\n        mov rdi, [rsp + pbytes]\n        mov rsi, rsp\n        call fp_mul2\n\n"
    S = S + "        1:\n        mov rdi, rsp\n        call fp_sq1\n\n        shr r13\n\n                inc r12\n                test r12, 64\n        jz 0b\n.endm\n\n"

    for i in range(0, pwords):
        S = S + "    POWSTEP " + str(i) + "\n"

    S = S + "    add rsp, pbytes+8\n    pop r13\n    pop r12\n    pop rbx\n    ret\n\n\n"

    print(S)
    return S

#////////////////////////////////////////
def PrintOthers():
    S = ".global fp_inv\nfp_inv:\n    lea rsi, [rip + p_minus_2]\n    jmp fp_pow\n\n\n"
    S = S + ".global fp_issquare\nfp_issquare:\n    push rdi\n    lea rsi, [rip + p_minus_1_halves]\n    call fp_pow\n    pop rdi\n\n"
    S = S + "    xor rax, rax\n    .set k, 0\n    .rept plimbs\n        mov rsi, [rdi + 8*k]\n        xor rsi, [rip + fp_1 + 8*k]\n        or rax, rsi\n"
    S = S + "         .set k, k+1\n    .endr\n    test rax, rax\n    setz al\n    movzx rax, al\n    ret\n\n\n"

    S = S + ".global fp_random\nfp_random:\n\n    push rdi\n    mov rsi, pbytes\n    call randombytes\n    pop rdi\n"
    S = S + "    mov rax, 1\n    shl rax, (pbits % 64)\n    dec rax\n    and [rdi + pbytes-8], rax\n\n    .set k, plimbs-1\n    .rept plimbs\n"
    S = S + "        mov rax, [rip + p + 8*k]\n        cmp [rdi + 8*k], rax\n        jge fp_random\n        jl 0f\n        .set k, k-1\n    .endr\n    0:\n    ret"

    print(S)
    return S

#////////////////////////////////////////
def Print_Parameters(p, pbits, pbytes, pwords):

#// Montgomery parameter R
    R = 2**(64*pwords)
    RR = IntegerModRing(R)

#//Extracting p words and saving in file
    P = WordExtractor(p, pwords, "p");

# // Computing -(1/p) mod R;
    #pinv = IntegerRing()!(RR!(-1/p))
    pinv = int(RR(-p).inverse_of_unit())
    #pinv = -(1//p)%R

# //Extracting pinv words and saving in file
    Pinv = WordExtractor(pinv, pwords, "inv_min_p_mod_r")

# // Printing zero
    S = ".global fp_0\nfp_0:\n.zero pbytes\n\n"
    print(S)
# PrintFile(Name, S);

# // 1 at Montgomery domain
    one = R%p

# //Extracting one words and saving in file
    ONE = WordExtractor(one, pwords, "fp_1")

# // R^2 at Montgomery domain
    Rsqr = R**2%p

# //Extracting Rsqr words and saving in file
    RsqrWords = WordExtractor(Rsqr, pwords, "r_squared_mod_p ")

# //Extracting p-2 words and saving in file
    pminus2Words = WordExtractor(p-2, pwords, "p_minus_2")

# //Extracting phalves words and saving in file
    phalvesWords = WordExtractor((p-1)//2, pwords, "p_minus_1_halves")

# //Extracting pquarters words and saving in file
    pquartersWords = WordExtractor((p-3)//4, pwords, "p_minus_3_quarters")

# //Printing footer
    S = PrintFooter()

    return 0

#////////////////////////////////////////
def Print_Assembly(p):

#// size of p in bits, bytes ,words
    pbits = math.ceil(math.log(p, 2.0))
    pbytes = math.ceil(pbits/8)
    if((pbytes%4) != 0):
        pbytes = pbytes + 4 - (pbytes%4)
    pwords = math.ceil(pbits/64)

#// Writing header;
    S = PrintHeader(pbits, pbytes, pwords)

#// Writing parameters;
    S = Print_Parameters(p, pbits, pbytes, pwords)

#// Printing Reduce Once
    S = PrintRedOnce(pbytes)

#// Printing Add_Sub
    S = PrintSub()

#// Printing Mult
    S = PrintMul(pbytes, pwords)

#// Printing Pow
    S = PrintPow(pwords)

#// Printing Others
    S = PrintOthers()

    return 0;


#//+++++++++++++++ Main ++++++++++++++++++//

# Primes of the form p := 2^e * prod(ell_i's) - 1
p_2047d221 = 0x5160D4543A2596D320C080B284C0FA5D3600AE4E29B85374858B238036139EA0B8B0C8B2850475382865FD4C9F7C3B5E531ED7D0FC022A13270300584EC78190FD09755A56CFEB1FC6961581CDFC56E824D0F31C4D4ECF04C5243CA0651820AF413023A7310203F74858FBECACA26B375BEBA9DE78CC420A069477B7FE595F83B148223C6841B3592C74AF79F39AE8F3D64F8B9FC946BB1C84A4541CBC2F363029B2C1E296158774A9646D2E186AD699B304FC7311F0DEC85E651756DDB4E3888D02333D591583AE5DB2F656E63A6179CDB059ED9BF90BAD614DCA5628C940C5004D99FB1CB03CE478F65726B12E42FA1C7C8FFFFFFFFFFFFFFFFFFFFFFFFFFF
# Primes of the form p := 2^e - c 
p_4095d221 = 0x7FFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFA5DCA635299EB2760CEDC3F12450C814FFA2DAFE875F09E8431F9BA694168EEF6A817F89E440736245D3D5615D75C648E8F90A3A4C6D12637CCD73F0B737F1AC09AF7B08CC11A71C4828DD80E16A08A29B57FB1322C6CF36BFD7B3C80A74000006EC3E89EA92275375DDAFD803BAF91F015F8218A5656A5785B5D8AAD338C9A4B887C3C7F367CC427744A1769686187B788DE2A6779C85A68380A7D2B7CC3B5FD680BC2708E20D188C0C320B3BF35B180FB234C85A0B72C5CE482CABE61EA58A6E2DD0FF11C5961E04B98980F9F1AF33B32E3E95171509454DFC9D28084903267459D8A3E22C44CB499384AB50DF44F65298B
p_5119d256 = 0x7FFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFE89A36D0A7F27637A69FA7A62E0497BC63E3F1C816F32AEAF17F7839D6750A178BB4B691A9922EAB7C9634D0F1491712278F84DA8FC1F472A05D101B8BE9CA1C44282F3AB0B081029CA3967363106F6FCB8542CFEBF9ED83C4B3A35DB0FC42E1FC21CFD1182E49B993A080BC3275B94EA17F8FBB0D7D4BCB3049D6C140A1869043F6460543CD4FEEDEED8D712D6419FE70166C3B8AECA00FACF147447A84B234E2851C58EAE204D626ACE863146FB74EB684C022F3BF3B8BF2CDA4F7A93742D0FB1BCEB40E0E8B01D90C4AFF7ED5B0050F67E9BF0C4A45B73197DE6FF68894CAE2C7B5846A541E178D4FCD073E1282F4488C33A0BFD91BFD26762233E12C84111AEB5BCD7C4C4A5ABFB99C416A83A8637CCF0F4A50FB6B4281CAF7EE5290777B47
p_6143d306 = 0x7FFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFF8BCBFA770CD08125807E305692E3C85B91BF0FD94B1AA66D620CD5AFDC3FD8B70B2F4A6D658EB2C108D85F2FC4A7F0EBA11711559966812FEAB4D8146C14F63D4F6C2DDF7F95E57D79A1708A70B938D443A33F6414F596193254946C057E55E0EB64468AA7C74E27D4262760A518B88CD7416F87BFD6B50C1FA2F9632E83D3532E4C54DF2610070E2260AA68098D41D32ED17EBEF25AD80291737FCE124CA40B14CD8D2A541D7E7EE43B59BE50C34D88B9DBDDF0913E701827191891890BA18B4442C4616112857E33749315DC057E74137968E068ED50E366915A615D18677497F66D95620EA399670B8957BCB31FF0D04FCD7420DB636851A46A1C8F8E9335FB5A3231D0CC421605228CD7544FAD284FB65FBAB19A1D943894FDD32A95F2D3A4EAF0C54C75070ED12E86EF0BD593B69D11DADCF4C4CC9AE8F9F58286E779E101D4E920E9EA883676B558C2E054F62AB63B52A0466D1D8D558ACE1EE2DE0305CB1A97233
p_8191d306 = 0x7FFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFB0005092EFE1954EBB40A5CC43278D477FD9E3EAB895B8F2DDBE5B21E0CFE797534FB57593D9CED0575EDB5B3C891BCD97F5C24D8225ED6BF593503E0E92D16B5817930E596246C7D9FA5C4E69BD697E5785E4029A6FCE266873B20072D9D77FBE3A6787D9505832B1C4CB8FBF8182DE87845FB05729FE0EB5D2E78E15A941D41085E07E5ABDBB810BA0B0CDA857D73EFEF39F186CA5B351388824A4401AD9C705758DD32F6A24BD885A87DE91F2ACB13EE80A20603BA5902790CB5A4DBA62746649523DE1E2E9EF976A1F71A5540135C50F4E2DA7E1DFBD362CC30CCEAEA80B9566007E22803F27E88038BA93DEFDCC41B1CC4AFE87D59271537740DB1E90108F580E913FDC38F9F2CA0009E839AB129D652A58322FAB0E170D65F12452E141AFE4B35F6ECFB3FC418A76092862349F03FBB55FA7711B3848BB2277A7F27EA7B2B5A1BEDF1BBF3302E59F5EC70E356FED1AE90629595BCA065091757DF032885E148C0DAB
p_9215d384 = 0x7FFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFCBA9E8D0D6E9647C535CB46390ED91FC7AB7043EB5A9C26EB56EBB88454A58C04553995209FDB2410C9C67866931FEAFC080BB4CC7FA187C8EA2BC31840C389EDBAEE5977D5A481BB874F0570EB0A90B188BF78E3116848B9F422B6BDC79FCFA5BEB1C951A307D90FA5A04E6D9BF026EBAFD375694158D9D49691E5D417608B3F969E83C4B10384F42A9D12ED50AA779290FB264E91A2E8C34A2DED0A3871578C01954350DF448D390A0E3F4A7B11F5B679A57CF74CDF1379FB85BC333D2984CD67DF800211ED3B655081ACF4283069139E3C1038A7E7CA1C359D6E1419E1595C3BCD44506EF8433ABA1C7C256A52C3798F0C512A845833F9CE9FE55E8DD29533658843F47960E92EE2CDDCFB5536967C535FE8CCC9E9C8D344E0F4E313378066BF91B95A5337EE185D4467A2C4D4B5997B38F79B7FB7B42C375B29A38B8F5C8069057CAC67235E887B4D8159BE72AE32E14D0FFD1B42A3FF326055B4C328CDAD6A75F5B23C399998F5DE800FEFECD8B3AFDAE00255184CB60E24E43A457BB6F7BE3D0421CEC679680564CE765691B9C8389AC3633D15CF94E29C45F1979022217553F152DB66127808BF4FFD6CD2BB454D63222DF2E9B7DC4A6ABDD88150FB50DA44CBAF5160B27205E82C68E055EF2EE83

# NIST LEVEL 1
#p = p_2047d221
p = p_4095d221
#p = p_5119d256
# NIST LEVEL 3
#p = p_6143d306
#p = p_8191d306
#p = p_9215d384

n = math.ceil(math.log(p, 2.0))

if (n % 64) != 0:
    n = n + 64 - (n%64)

Print_Assembly(p)
#PrintUintHeader(n//64)