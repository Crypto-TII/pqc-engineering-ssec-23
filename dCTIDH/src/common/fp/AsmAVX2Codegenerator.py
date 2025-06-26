#!/usr/bin/env sage -python


# how to call
# sage -python AsmMultCodegenerator.py  > fp9216.s

#from sage.all import *
import math
import sys

sefOfLimbs = set()
printOut = 0

def rotate(l, x):
    return l[-x:] + l[:-x]

def push():
    S = "# -------------------\n"
    S = S + "# push\n"
    # S = S + "    push rbx\n    push rbp\n    push rdi\n    push rsi\n    push r12\n    push r13\n    push r14\n    push r15\n\n"
    S = S + "    push rbx\n    push rbp\n    push rsi\n    push r12\n    push r13\n    push r14\n    push r15\n\n"    
    # S = S + "    push rdx\n    push rdi\n    push rsi\n\n"    
    return S

def pop():
    S = "# -------------------\n"
    S = S + "# pop\n"    
    # S = S + "    pop r15\n    pop r14\n    pop r13\n    pop r12\n    pop rsi\n    pop rdi\n    pop rbp\n    pop rbx\n\n"
    S = S + "    pop r15\n    pop r14\n    pop r13\n    pop r12\n    pop rsi\n    pop rbp\n    pop rbx\n\n"    
    # S = S + "    pop rsi\n    pop rdi\n    pop rdx\n\n"    
    return S


def PrintSquaringInterleaved(plimbs):

    # registers reserved rdi, rsi, rdx
    # rax, rbx = rcx, r8
    registers = ["ymm0", "ymm1", "ymm2", "ymm3", "ymm4", "ymm5", "ymm6", "ymm7", "ymm8", "ymm9", "ymm10", "ymm11", "ymm12"]
    reg_locked = [0] * len(registers)
    perm = False
    count = 0

    # ymm13 = temp shuffle
    # ymm14 = temp register t0
    # ymm15 = current b

    if(plimbs > len(registers)):
        print("ERROR: Index out range")
        exit()
    state = registers[:plimbs]
    #state = registers

    S = ""
    # S = ".intel_syntax noprefix\n\n"
    # S = S + ".section .rodata\n\n"
    # S = S + ".section .text\n\n"

    S = S + ".global mult_avx2_"+ str(plimbs) + "x" + str(plimbs) + "\n"
    S = S + "mult_avx2_"+ str(plimbs) + "x" + str(plimbs) + ":\n"
    
    # load as much a's as possible
    countAs = min(len(registers) - plimbs, math.ceil(plimbs/2))
    S = S + "############## load as much a's as possible\n"
    for j in range(0, countAs):
        S = S + "    vmovdqa " + registers[j] + ", YMMWORD PTR [rsi + " + str(j*32) + "] \n"
        reg_locked[j] = 1

    for j in range(0,plimbs):
        if(j%2==1):
            S = S + "\n\n############## perm to b_" + str(j) + "\n"
            S = S + "    vpshufd ymm15, ymm15, 177\n"
            perm = True
        else:
            S = S + "\n\n############## load    b_" + str(j) + "\n"
            S = S + "    vmovdqa ymm15, YMMWORD PTR [rdx + " + str(j//2*32) + "] \n"     
            perm = False     
        if(j==0): # without adds = only for b0
            S = S + "############## w/o adds\n"    
            for k in range(0,plimbs):
                if(k==j):
                    S = S + "############## EQUAL\n"  
                S = S + "############## a_" + str(k) + " * b_" + str(j) + "\n"       
                if(k<countAs*2):
                    S = S + "############## read from regs\n"
                    if(k%2==1):
                        # S = S + "############## perm to a_" + str(k) + "\n"
                        S = S + "############## perm " + registers[k//2] + " to a_" + str(k) + "\n"
                        S = S + "    vpshufd " + registers[k//2] + ", " + registers[k//2] + ", 177\n"
                    else:
                        # S = S + "############## load    a_" + str(k) + "\n"
                        S = S + "############## " + registers[k//2] + " = a_" + str(k) + "\n"

                    S = S + "    vpmuldq " + registers[len(registers)-1 - ((j+k)%plimbs)] +" , ymm15, " + registers[k//2] + "\n"    
                else:
                    S = S + "############## read from stack\n"
                    if(k%2==1):
                        S = S + "############## perm to a_" + str(k) + "\n"
                        S = S + "    vpshufd ymm13, ymm13, 177\n"
                    else:
                        S = S + "    vmovdqa ymm13, YMMWORD PTR [rsi + " +  str((k//2)*32) + "]\n"                        
                        S = S + "############## load    a_" + str(k) + "\n"  
                    S = S + "    vpmuldq " + registers[len(registers)-1 - ((j+k)%plimbs)] +", ymm15, ymm13\n"  
            S = S + "    vmovdqu YMMWORD PTR [rdi + " + str(j*32) + "], " + registers[len(registers)-1 - j] + "\n"  
            S = S + "    vxorps " + registers[len(registers)-1 - j] + ", " + registers[len(registers)-1 - j] + ", " + registers[len(registers)-1 - j] + "\n"                
            # S = S + "    vpxor " + registers[len(registers)-1 - j] + ", " + registers[len(registers)-1 - j] + ", " + registers[len(registers)-1 - j] + "\n"                              
        else: # with adds
            S = S + "############## perm = " + str(perm) + "\n"
            S = S + "############## w/ adds\n"
            for k in range(0,plimbs):
                if(k<countAs*2):
                    if(perm):
                        if(k%2==1):
                            S = S + "############## a_" + str(k-1) + " * b_" + str(j) + "\n"
                        else:
                            S = S + "############## a_" + str(k+1) + " * b_" + str(j) + "\n"
                    else:
                        S = S + "############## a_" + str(k) + " * b_" + str(j) + "\n"                    
                    S = S + "############## read from regs\n"
                    if(perm):
                        if(k%2==1):
                            S = S + "############## perm " + registers[k//2] + " to a_" + str(k-1) + "\n"
                            S = S + "    vpshufd " + registers[k//2] + ", " + registers[k//2] + ", 177\n"                              
                            if(k==plimbs-2):
                                S = S + "############## save add\n"

                            S = S + "    vpmuldq ymm14, ymm15, " + registers[k//2] + "\n"                                                      
                            S = S + "    vpaddq  " + registers[len(registers)-1 - ((j+k-1)%plimbs)] + ", ymm14, " + registers[len(registers)-1 - ((j+k-1)%plimbs)] + " \n"                                
                        else:
                            if(k==plimbs-2):
                                S = S + "############## save add\n"
                            S = S + "############## " + registers[k//2] + " = a_" + str(k+1) + "\n"
                            S = S + "    vpmuldq ymm14, ymm15, " + registers[k//2] + "\n"                               
                            S = S + "    vpaddq  " + registers[len(registers)-1 - ((j+k+1)%plimbs)] + ", ymm14, " + registers[len(registers)-1 - ((j+k+1)%plimbs)] + " \n"                                                                                                   
                    else:
                        if(k%2==1):
                            if(k==plimbs-2):
                                S = S + "############## save add\n"
                            S = S + "############## perm " + registers[k//2] + " to a_" + str(k) + "\n"
                            S = S + "    vpshufd " + registers[k//2] + ", " + registers[k//2] + ", 177\n"                              
                            S = S + "    vpmuldq ymm14, ymm15, " + registers[k//2] + "\n"                                                      
                            S = S + "    vpaddq  " + registers[len(registers)-1 - ((j+k)%plimbs)] + ", ymm14, " + registers[len(registers)-1 - ((j+k)%plimbs)] + " \n"                            
                        else:
                            S = S + "############## " + registers[k//2] + " = a_" + str(k) + "\n"
                            S = S + "    vpmuldq ymm14, ymm15, " + registers[k//2] + "\n"                               
                            S = S + "    vpaddq  " + registers[len(registers)-1 - ((j+k)%plimbs)] + ", ymm14, " + registers[len(registers)-1 - ((j+k)%plimbs)] + " \n"                             

                    
                else:
                    S = S + "############## a_" + str(k) + " * b_" + str(j) + "\n"    
                    S = S + "############## read from stack\n"
                    if(k%2==1):
                        S = S + "############## perm to a_" + str(k) + "\n"
                        S = S + "    vpshufd ymm13, ymm13, 177\n"
                    else:

                        S = S + "############## load    a_" + str(k) + "\n"
                        S = S + "    vmovdqa ymm13, YMMWORD PTR [rsi + " +  str((k//2)*32) + "]\n"                 
                    if(k==plimbs-1):
                        S = S + "############## save add\n"
                        S = S + "    vpmuldq " + registers[len(registers)-1 - ((j+k)%plimbs)] + ", ymm15, ymm13\n"                        
                    else:
                        S = S + "    vpmuldq ymm14, ymm15, ymm13\n"
                        S = S + "    vpaddq  " + registers[len(registers)-1 - ((j+k)%plimbs)] + ", ymm14, " + registers[len(registers)-1 - ((j+k)%plimbs)] +"\n"                                                    
                       

            S = S + "    vmovdqu YMMWORD PTR [rdi + " + str(j*32) + "] , " + registers[len(registers)-1 - j] + "\n" 
            S = S + "    vxorps " + registers[len(registers)-1 - j] + ", " + registers[len(registers)-1 - j] + ", " + registers[len(registers)-1 - j] + "\n"
            # S = S + "    vpxor " + registers[len(registers)-1 - j] + ", " + registers[len(registers)-1 - j] + ", " + registers[len(registers)-1 - j] + "\n"   

    S = S + "############## write all the rest\n"
    for j in range(0,plimbs-1):
        S = S + "    vmovdqu YMMWORD PTR [rdi + " + str((plimbs + j)*32) + "], " + registers[len(registers)- j - 1] + "\n"

    S = S + "    ret\n"

    print(S)

    return 0


def PrintMultInterleaved(plimbs):

    # registers reserved rdi, rsi, rdx
    # rax, rbx = rcx, r8
    registers = ["ymm0", "ymm1", "ymm2", "ymm3", "ymm4", "ymm5", "ymm6", "ymm7", "ymm8", "ymm9", "ymm10", "ymm11", "ymm12"]
    reg_locked = [0] * len(registers)
    perm = False
    count = 0

    # ymm13 = temp shuffle
    # ymm14 = temp register t0
    # ymm15 = current b

    if(plimbs > len(registers)):
        print("ERROR: Index out range")
        exit()
    state = registers[:plimbs]
    #state = registers

    S = ""
    # S = ".intel_syntax noprefix\n\n"
    # S = S + ".section .rodata\n\n"
    # S = S + ".section .text\n\n"

    S = S + ".global mult_avx2_"+ str(plimbs) + "x" + str(plimbs) + "\n"
    S = S + "mult_avx2_"+ str(plimbs) + "x" + str(plimbs) + ":\n"
    
    # load as much a's as possible
    countAs = min(len(registers) - plimbs, math.ceil(plimbs/2))
    S = S + "############## load as much a's as possible\n"
    for j in range(0, countAs):
        S = S + "    vmovdqa " + registers[j] + ", YMMWORD PTR [rsi + " + str(j*32) + "] \n"
        reg_locked[j] = 1

    for j in range(0,plimbs):
        if(j%2==1):
            S = S + "\n\n############## perm to b_" + str(j) + "\n"
            S = S + "    vpshufd ymm15, ymm15, 177\n"
            perm = True
        else:
            S = S + "\n\n############## load    b_" + str(j) + "\n"
            S = S + "    vmovdqa ymm15, YMMWORD PTR [rdx + " + str(j//2*32) + "] \n"     
            perm = False     
        if(j==0): # without adds = only for b0
            S = S + "############## w/o adds\n"
    
            for k in range(0,plimbs):
                S = S + "############## a_" + str(k) + " * b_" + str(j) + "\n"       
                if(k<countAs*2):
                    S = S + "############## read from regs\n"
                    if(k%2==1):
                        # S = S + "############## perm to a_" + str(k) + "\n"
                        S = S + "############## perm " + registers[k//2] + " to a_" + str(k) + "\n"
                        S = S + "    vpshufd " + registers[k//2] + ", " + registers[k//2] + ", 177\n"
                    else:
                        # S = S + "############## load    a_" + str(k) + "\n"
                        S = S + "############## " + registers[k//2] + " = a_" + str(k) + "\n"

                    S = S + "    vpmuldq " + registers[len(registers)-1 - ((j+k)%plimbs)] +" , ymm15, " + registers[k//2] + "\n"    
                else:
                    S = S + "############## read from stack\n"
                    if(k%2==1):
                        S = S + "############## perm to a_" + str(k) + "\n"
                        S = S + "    vpshufd ymm13, ymm13, 177\n"
                    else:
                        S = S + "    vmovdqa ymm13, YMMWORD PTR [rsi + " +  str((k//2)*32) + "]\n"                        
                        S = S + "############## load    a_" + str(k) + "\n"  
                    S = S + "    vpmuldq " + registers[len(registers)-1 - ((j+k)%plimbs)] +", ymm15, ymm13\n"  
            S = S + "    vmovdqu YMMWORD PTR [rdi + " + str(j*32) + "], " + registers[len(registers)-1 - j] + "\n"  
            S = S + "    vxorps " + registers[len(registers)-1 - j] + ", " + registers[len(registers)-1 - j] + ", " + registers[len(registers)-1 - j] + "\n"                
            # S = S + "    vpxor " + registers[len(registers)-1 - j] + ", " + registers[len(registers)-1 - j] + ", " + registers[len(registers)-1 - j] + "\n"                              
        else: # with adds
            S = S + "############## perm = " + str(perm) + "\n"
            S = S + "############## w/ adds\n"
            for k in range(0,plimbs):
                if(k<countAs*2):
                    if(perm):
                        if(k%2==1):
                            S = S + "############## a_" + str(k-1) + " * b_" + str(j) + "\n"
                        else:
                            S = S + "############## a_" + str(k+1) + " * b_" + str(j) + "\n"
                    else:
                        S = S + "############## a_" + str(k) + " * b_" + str(j) + "\n"                    
                    S = S + "############## read from regs\n"
                    if(perm):
                        if(k%2==1):
                            S = S + "############## perm " + registers[k//2] + " to a_" + str(k-1) + "\n"
                            S = S + "    vpshufd " + registers[k//2] + ", " + registers[k//2] + ", 177\n"                              
                            if(k==plimbs-2):
                                S = S + "############## save add\n"

                            S = S + "    vpmuldq ymm14, ymm15, " + registers[k//2] + "\n"                                                      
                            S = S + "    vpaddq  " + registers[len(registers)-1 - ((j+k-1)%plimbs)] + ", ymm14, " + registers[len(registers)-1 - ((j+k-1)%plimbs)] + " \n"                                
                        else:
                            if(k==plimbs-2):
                                S = S + "############## save add\n"
                            S = S + "############## " + registers[k//2] + " = a_" + str(k+1) + "\n"
                            S = S + "    vpmuldq ymm14, ymm15, " + registers[k//2] + "\n"                               
                            S = S + "    vpaddq  " + registers[len(registers)-1 - ((j+k+1)%plimbs)] + ", ymm14, " + registers[len(registers)-1 - ((j+k+1)%plimbs)] + " \n"                                                                                                   
                    else:
                        if(k%2==1):
                            if(k==plimbs-2):
                                S = S + "############## save add\n"
                            S = S + "############## perm " + registers[k//2] + " to a_" + str(k) + "\n"
                            S = S + "    vpshufd " + registers[k//2] + ", " + registers[k//2] + ", 177\n"                              
                            S = S + "    vpmuldq ymm14, ymm15, " + registers[k//2] + "\n"                                                      
                            S = S + "    vpaddq  " + registers[len(registers)-1 - ((j+k)%plimbs)] + ", ymm14, " + registers[len(registers)-1 - ((j+k)%plimbs)] + " \n"                            
                        else:
                            S = S + "############## " + registers[k//2] + " = a_" + str(k) + "\n"
                            S = S + "    vpmuldq ymm14, ymm15, " + registers[k//2] + "\n"                               
                            S = S + "    vpaddq  " + registers[len(registers)-1 - ((j+k)%plimbs)] + ", ymm14, " + registers[len(registers)-1 - ((j+k)%plimbs)] + " \n"                             

                    
                else:
                    S = S + "############## a_" + str(k) + " * b_" + str(j) + "\n"    
                    S = S + "############## read from stack\n"
                    if(k%2==1):
                        S = S + "############## perm to a_" + str(k) + "\n"
                        S = S + "    vpshufd ymm13, ymm13, 177\n"
                    else:

                        S = S + "############## load    a_" + str(k) + "\n"
                        S = S + "    vmovdqa ymm13, YMMWORD PTR [rsi + " +  str((k//2)*32) + "]\n"                 
                    if(k==plimbs-1):
                        S = S + "############## save add\n"
                        S = S + "    vpmuldq " + registers[len(registers)-1 - ((j+k)%plimbs)] + ", ymm15, ymm13\n"                        
                    else:
                        S = S + "    vpmuldq ymm14, ymm15, ymm13\n"
                        S = S + "    vpaddq  " + registers[len(registers)-1 - ((j+k)%plimbs)] + ", ymm14, " + registers[len(registers)-1 - ((j+k)%plimbs)] +"\n"                                                    
                       

            S = S + "    vmovdqu YMMWORD PTR [rdi + " + str(j*32) + "] , " + registers[len(registers)-1 - j] + "\n" 
            S = S + "    vxorps " + registers[len(registers)-1 - j] + ", " + registers[len(registers)-1 - j] + ", " + registers[len(registers)-1 - j] + "\n"
            # S = S + "    vpxor " + registers[len(registers)-1 - j] + ", " + registers[len(registers)-1 - j] + ", " + registers[len(registers)-1 - j] + "\n"   

    S = S + "############## write all the rest\n"
    for j in range(0,plimbs-1):
        S = S + "    vmovdqu YMMWORD PTR [rdi + " + str((plimbs + j)*32) + "], " + registers[len(registers)- j - 1] + "\n"

    S = S + "    ret\n"

    print(S)

    return 0


def Print2xAddAVX2(plimbs):

    S = ""
    # add_2x_2x2_avx2
    S = S + ".global add_2x_"+ str(plimbs) + "x" + str(plimbs) + "_avx2\n"

    S = S + "add_2x_"+ str(plimbs) + "x" + str(plimbs) + "_avx2:\n"
    for i in range(0, plimbs):
        S = S + "        vmovdqa ymm0, ymmword ptr [rsi + " + str(plimbs*32 + i*32) + "]\n"
        S = S + "        vpaddd  ymm0, ymm0, ymmword ptr [rsi + " + str(i*32) + "]\n"
        S = S + "        vmovdqa ymmword ptr [rdi + " + str(i*32) + "], ymm0\n"

        S = S + "        vmovdqa ymm1, ymmword ptr [rdx + " +  str(plimbs*32 + i*32)+ "]\n"
        S = S + "        vpaddd  ymm1, ymm1, ymmword ptr [rdx + " + str(i*32) + "]\n"
        S = S + "        vmovdqa ymmword ptr [rdi + " + str(i*32+(plimbs*32)) + "], ymm1\n"    

    S = S + "    ret\n"    

    print(S)

    return 0

def PrintSubAVX2(plimbs):
    
    registers = ["ymm0", "ymm1", "ymm2", "ymm3", "ymm4", "ymm5", "ymm6", "ymm7", "ymm8", "ymm9", "ymm10", "ymm11", "ymm12", "ymm13"]

    S = ""
    # add_2x_2x2_avx2
    S = S + ".global sub_"+ str(plimbs) + "x" + str(plimbs) + "_avx2\n"

    S = S + "sub_"+ str(plimbs) + "x" + str(plimbs) + "_avx2:\n"
    for i in range(0, plimbs):
        S = S + "        vmovdqa " + registers[i%len(registers)] + ", ymmword ptr [rdi + " + str(i*32) + "]\n"
        S = S + "        vpsubd  " + registers[i%len(registers)] + ", " + registers[i%len(registers)] + ", ymmword ptr [rsi + " + str(i*32) + "]\n"
        S = S + "        vpsubd  " + registers[i%len(registers)] + ", " + registers[i%len(registers)] + ", ymmword ptr [rdx + " + str(i*32) + "]\n"        
        S = S + "        vmovdqa ymmword ptr [rdi + " + str(i*32) + "], " + registers[i%len(registers)] + "\n"

    S = S + "    ret\n"    

    print(S)

    return 0

def PrintInterleave_4x_10x10():

    S = ""

    S = S + ".global interleave_4x_10x10_avx2\n"
    S = S + "interleave_4x_10x10_avx2:\n"
    S = S + "        vmovdqa ymm5, YMMWORD PTR [rdx]\n"
    S = S + "        vmovdqa ymm7, YMMWORD PTR [r8]\n"
    S = S + "        vperm2i128      ymm3, ymm5, YMMWORD PTR [rsi], 2\n"
    S = S + "        vperm2i128      ymm2, ymm7, YMMWORD PTR [rcx], 2\n"
    S = S + "        vperm2i128      ymm1, ymm5, YMMWORD PTR [rsi], 19\n"
    S = S + "        vperm2i128      ymm0, ymm7, YMMWORD PTR [rcx], 19\n"
    S = S + "        vpermq  ymm3, ymm3, 216\n"
    S = S + "        vpermq  ymm2, ymm2, 216\n"
    S = S + "        vpermq  ymm1, ymm1, 216\n"
    S = S + "        vperm2i128      ymm4, ymm2, ymm3, 2\n"
    S = S + "        vpermq  ymm0, ymm0, 216\n"
    S = S + "        vperm2i128      ymm2, ymm2, ymm3, 19\n"
    S = S + "        vmovdqa YMMWORD PTR [rdi], ymm4\n"
    S = S + "        vmovdqa YMMWORD PTR [rdi+32], ymm2\n"
    S = S + "        vperm2i128      ymm2, ymm0, ymm1, 2\n"
    S = S + "        vperm2i128      ymm0, ymm0, ymm1, 19\n"
    S = S + "        vmovdqa YMMWORD PTR [rdi+64], ymm2\n"
    S = S + "        vmovdqa YMMWORD PTR [rdi+96], ymm0\n"
    S = S + "        vmovdqa ymm1, YMMWORD PTR [rdx+32]\n"
    S = S + "        vmovdqa ymm0, YMMWORD PTR [r8+32]\n"
    S = S + "        vperm2i128      ymm1, ymm1, YMMWORD PTR [rsi+32], 2\n"
    S = S + "        vperm2i128      ymm0, ymm0, YMMWORD PTR [rcx+32], 2\n"
    S = S + "        vpermq  ymm1, ymm1, 216\n"
    S = S + "        vpermq  ymm0, ymm0, 216\n"
    S = S + "        vperm2i128      ymm0, ymm0, ymm1, 2\n"
    S = S + "        vmovdqa YMMWORD PTR [rdi+128], ymm0\n"
    S = S + "        ret\n"
    print(S)

    return 0


def PrintDeInterleave_4x_10x10():
    
    S = ""

    S = S + ".global deinterleave_4x_10x10_avx2\n"
    S = S + "deinterleave_4x_10x10_avx2:\n"

    S = S + "        vmovdqa ymm0, ymmword ptr [rdi + 32]\n"
    S = S + "        vpsllq  ymm0, ymm0, 32\n"
    S = S + "        vpxor   ymm0, ymm0, ymmword ptr [rdi]\n"
    S = S + "        vmovdqa ymm1, ymmword ptr [rdi + 96]\n"
    S = S + "        vpsllq  ymm1, ymm1, 32\n"
    S = S + "        vpxor   ymm1, ymm1, ymmword ptr [rdi + 64]\n"
    S = S + "        vmovdqa ymm2, ymmword ptr [rdi + 160]\n"
    S = S + "        vpsllq  ymm2, ymm2, 32\n"
    S = S + "        vpxor   ymm2, ymm2, ymmword ptr [rdi + 128]\n"
    S = S + "        vmovdqa ymm3, ymmword ptr [rdi + 224]\n"
    S = S + "        vpsllq  ymm3, ymm3, 32\n"
    S = S + "        vpxor   ymm3, ymm3, ymmword ptr [rdi + 192]\n"
    S = S + "        vinserti128     ymm4, ymm0, xmm1, 1\n"
    S = S + "        vmovdqa ymmword ptr [rsi], ymm4\n"
    S = S + "        vperm2i128      ymm0, ymm0, ymm1, 49 \n"
    S = S + "        vmovdqa ymmword ptr [rdx], ymm0\n"
    S = S + "        vinserti128     ymm0, ymm2, xmm3, 1\n"
    S = S + "        vmovdqa ymmword ptr [rcx], ymm0\n"
    S = S + "        vperm2i128      ymm0, ymm2, ymm3, 49\n"
    S = S + "        vmovdqa ymmword ptr [r8], ymm0\n"
    S = S + "        vpermpd ymm1, ymmword ptr [rsi], 216\n"
    S = S + "        vpermq  ymm2, ymmword ptr [rdx], 216\n"
    S = S + "        vpermpd ymm3, ymmword ptr [rcx], 216\n"
    S = S + "        vpermq  ymm0, ymm0, 216 \n"
    S = S + "        vinsertf128     ymm4, ymm1, xmm3, 1\n"
    S = S + "        vmovaps ymmword ptr [rsi], ymm4\n"
    S = S + "        vperm2f128      ymm1, ymm1, ymm3, 49   \n"
    S = S + "        vmovaps ymmword ptr [rdx], ymm1\n"
    S = S + "        vinserti128     ymm1, ymm2, xmm0, 1\n"
    S = S + "        vmovdqa ymmword ptr [rcx], ymm1\n"
    S = S + "        vperm2i128      ymm0, ymm2, ymm0, 49   \n"
    S = S + "        vmovdqa ymmword ptr [r8], ymm0\n"
    S = S + "        vmovdqa ymm0, ymmword ptr [rdi + 288]\n"
    S = S + "        vpsllq  ymm0, ymm0, 32\n"
    S = S + "        vpxor   ymm0, ymm0, ymmword ptr [rdi + 256]\n"
    S = S + "        vmovq   xmm1, xmm0 \n"
    S = S + "        vmovdqa ymmword ptr [rsi + 32], ymm1\n"
    S = S + "        vpsrldq xmm1, xmm0, 8 \n"
    S = S + "        vmovdqa ymmword ptr [rdx + 32], ymm1\n"
    S = S + "        vextracti128    xmm1, ymm0, 1\n"
    S = S + "        vmovq   xmm1, xmm1 \n"
    S = S + "        vmovdqa ymmword ptr [rcx + 32], ymm1\n"
    S = S + "        vpermq  ymm0, ymm0, 255\n"
    S = S + "        vmovq   xmm0, xmm0\n"
    S = S + "        vmovdqa ymmword ptr [r8 + 32], ymm0\n"
    S = S + "        vmovdqa ymm0, ymmword ptr [rdi + 352]\n"
    S = S + "        vmovdqa ymm1, ymmword ptr [rdi + 416]\n"
    S = S + "        vmovdqa ymm2, ymmword ptr [rdi + 480]\n"
    S = S + "        vpsllq  ymm0, ymm0, 32\n"
    S = S + "        vpxor   ymm0, ymm0, ymmword ptr [rdi + 320]\n"
    S = S + "        vmovdqa ymm3, ymmword ptr [rdi + 544]\n"
    S = S + "        vpsllq  ymm1, ymm1, 32\n"
    S = S + "        vpxor   ymm1, ymm1, ymmword ptr [rdi + 384]\n"
    S = S + "        vpsllq  ymm2, ymm2, 32\n"
    S = S + "        vpxor   ymm2, ymm2, ymmword ptr [rdi + 448]\n"
    S = S + "        vpsllq  ymm3, ymm3, 32\n"
    S = S + "        vpxor   ymm3, ymm3, ymmword ptr [rdi + 512]\n"
    S = S + "        vinserti128     ymm4, ymm0, xmm1, 1\n"
    S = S + "        vmovdqa ymmword ptr [rsi + 64], ymm4\n"
    S = S + "        vperm2i128      ymm0, ymm0, ymm1, 49    # ymm0 = ymm0[2,3],ymm1[2,3]\n"
    S = S + "        vmovdqa ymmword ptr [rdx + 64], ymm0\n"
    S = S + "        vinserti128     ymm0, ymm2, xmm3, 1\n"
    S = S + "        vmovdqa ymmword ptr [rcx + 64], ymm0\n"
    S = S + "        vperm2i128      ymm0, ymm2, ymm3, 49    # ymm0 = ymm2[2,3],ymm3[2,3]\n"
    S = S + "        vmovdqa ymmword ptr [r8 + 64], ymm0\n"
    S = S + "        vpermpd ymm1, ymmword ptr [rsi + 64], 216 # ymm1 = mem[0,2,1,3]\n"
    S = S + "        vpermq  ymm2, ymmword ptr [rdx + 64], 216 # ymm2 = mem[0,2,1,3]\n"
    S = S + "        vpermpd ymm3, ymmword ptr [rcx + 64], 216 # ymm3 = mem[0,2,1,3]\n"
    S = S + "        vpermq  ymm0, ymm0, 216\n"
    S = S + "        vinsertf128     ymm4, ymm1, xmm3, 1\n"
    S = S + "        vmovaps ymmword ptr [rsi + 64], ymm4\n"
    S = S + "        vperm2f128      ymm1, ymm1, ymm3, 49    # ymm1 = ymm1[2,3],ymm3[2,3]\n"
    S = S + "        vmovaps ymmword ptr [rdx + 64], ymm1\n"
    S = S + "        vinserti128     ymm1, ymm2, xmm0, 1\n"
    S = S + "        vmovdqa ymmword ptr [rcx + 64], ymm1\n"
    S = S + "        vperm2i128      ymm0, ymm2, ymm0, 49    # ymm0 = ymm2[2,3],ymm0[2,3]\n"
    S = S + "        vmovdqa ymmword ptr [r8 + 64], ymm0\n"
    S = S + "        vmovdqa ymm0, ymmword ptr [rdi + 608]\n"
    S = S + "        vpsllq  ymm0, ymm0, 32\n"
    S = S + "        vpxor   ymm0, ymm0, ymmword ptr [rdi + 576]\n"
    S = S + "        vmovq   xmm1, xmm0 \n"
    S = S + "        vmovdqa ymmword ptr [rsi + 96], ymm1\n"
    S = S + "        vpsrldq xmm1, xmm0, 8\n"
    S = S + "        vmovdqa ymmword ptr [rdx + 96], ymm1\n"
    S = S + "        vextracti128    xmm1, ymm0, 1\n"
    S = S + "        vmovq   xmm1, xmm1\n"
    S = S + "        vmovdqa ymmword ptr [rcx + 96], ymm1\n"
    S = S + "        vpermq  ymm0, ymm0, 255\n"
    S = S + "        vmovq   xmm0, xmm0\n"
    S = S + "        vmovdqa ymmword ptr [r8 + 96], ymm0\n"
    S = S + "        ret\n"     
    print(S)

    return 0    

def PrintCarryVertical64():

    S = ""

    S = S + ".global carryVertical64_avx2\n"
    S = S + "carryVertical64_avx2:\n"    

    S = S + "        vmovdqa ymm1, ymmword ptr [rdi]\n"
    S = S + "        vpsrlq  ymm2, ymm1, 26\n"
    S = S + "        vpbroadcastq    ymm0, qword ptr [rip + .LCPI0_0] # ymm0 = [67108863,67108863,67108863,67108863]\n"
    S = S + "        vpand   ymm1, ymm1, ymm0\n"
    S = S + "        vmovdqa ymmword ptr [rdi], ymm1\n"
    S = S + "        vpaddq  ymm1, ymm2, ymmword ptr [rdi + 32]\n"
    S = S + "        vpsrlq  ymm2, ymm1, 26\n"
    S = S + "        vpand   ymm1, ymm1, ymm0\n"
    S = S + "        vmovdqa ymmword ptr [rdi + 32], ymm1\n"
    S = S + "        vpaddq  ymm1, ymm2, ymmword ptr [rdi + 64]\n"
    S = S + "        vpsrlq  ymm2, ymm1, 26\n"
    S = S + "        vpand   ymm1, ymm1, ymm0\n"
    S = S + "        vmovdqa ymmword ptr [rdi + 64], ymm1\n"
    S = S + "        vpaddq  ymm1, ymm2, ymmword ptr [rdi + 96]\n"
    S = S + "        vpsrlq  ymm2, ymm1, 26\n"
    S = S + "        vpand   ymm1, ymm1, ymm0\n"
    S = S + "        vmovdqa ymmword ptr [rdi + 96], ymm1\n"
    S = S + "        vpaddq  ymm1, ymm2, ymmword ptr [rdi + 128]\n"
    S = S + "        vpsrlq  ymm2, ymm1, 26\n"
    S = S + "        vpand   ymm1, ymm1, ymm0\n"
    S = S + "        vmovdqa ymmword ptr [rdi + 128], ymm1\n"
    S = S + "        vpaddq  ymm1, ymm2, ymmword ptr [rdi + 160]\n"
    S = S + "        vpsrlq  ymm2, ymm1, 26\n"
    S = S + "        vpand   ymm1, ymm1, ymm0\n"
    S = S + "        vmovdqa ymmword ptr [rdi + 160], ymm1\n"
    S = S + "        vpaddq  ymm1, ymm2, ymmword ptr [rdi + 192]\n"
    S = S + "        vpsrlq  ymm2, ymm1, 26\n"
    S = S + "        vpand   ymm1, ymm1, ymm0\n"
    S = S + "        vmovdqa ymmword ptr [rdi + 192], ymm1\n"
    S = S + "        vpaddq  ymm1, ymm2, ymmword ptr [rdi + 224]\n"
    S = S + "        vpsrlq  ymm2, ymm1, 26\n"
    S = S + "        vpand   ymm1, ymm1, ymm0\n"
    S = S + "        vmovdqa ymmword ptr [rdi + 224], ymm1\n"
    S = S + "        vpaddq  ymm1, ymm2, ymmword ptr [rdi + 256]\n"
    S = S + "        vpsrlq  ymm2, ymm1, 26\n"
    S = S + "        vpand   ymm1, ymm1, ymm0\n"
    S = S + "        vmovdqa ymmword ptr [rdi + 256], ymm1\n"
    S = S + "        vpaddq  ymm1, ymm2, ymmword ptr [rdi + 288]\n"
    S = S + "        vpsrlq  ymm2, ymm1, 26\n"
    S = S + "        vpand   ymm1, ymm1, ymm0\n"
    S = S + "        vmovdqa ymmword ptr [rdi + 288], ymm1\n"
    S = S + "        vpaddq  ymm1, ymm2, ymmword ptr [rdi + 320]\n"
    S = S + "        vpsrlq  ymm2, ymm1, 26\n"
    S = S + "        vpand   ymm1, ymm1, ymm0\n"
    S = S + "        vmovdqa ymmword ptr [rdi + 320], ymm1\n"
    S = S + "        vpaddq  ymm1, ymm2, ymmword ptr [rdi + 352]\n"
    S = S + "        vpsrlq  ymm2, ymm1, 26\n"
    S = S + "        vpand   ymm1, ymm1, ymm0\n"
    S = S + "        vmovdqa ymmword ptr [rdi + 352], ymm1\n"
    S = S + "        vpaddq  ymm1, ymm2, ymmword ptr [rdi + 384]\n"
    S = S + "        vpsrlq  ymm2, ymm1, 26\n"
    S = S + "        vpand   ymm1, ymm1, ymm0\n"
    S = S + "        vmovdqa ymmword ptr [rdi + 384], ymm1\n"
    S = S + "        vpaddq  ymm1, ymm2, ymmword ptr [rdi + 416]\n"
    S = S + "        vpsrlq  ymm2, ymm1, 26\n"
    S = S + "        vpand   ymm1, ymm1, ymm0\n"
    S = S + "        vmovdqa ymmword ptr [rdi + 416], ymm1\n"
    S = S + "        vpaddq  ymm1, ymm2, ymmword ptr [rdi + 448]\n"
    S = S + "        vpsrlq  ymm2, ymm1, 26\n"
    S = S + "        vpand   ymm1, ymm1, ymm0\n"
    S = S + "        vmovdqa ymmword ptr [rdi + 448], ymm1\n"
    S = S + "        vpaddq  ymm1, ymm2, ymmword ptr [rdi + 480]\n"
    S = S + "        vpsrlq  ymm2, ymm1, 26\n"
    S = S + "        vpand   ymm1, ymm1, ymm0\n"
    S = S + "        vmovdqa ymmword ptr [rdi + 480], ymm1\n"
    S = S + "        vpaddq  ymm1, ymm2, ymmword ptr [rdi + 512]\n"
    S = S + "        vpsrlq  ymm2, ymm1, 26\n"
    S = S + "        vpand   ymm1, ymm1, ymm0\n"
    S = S + "        vmovdqa ymmword ptr [rdi + 512], ymm1\n"
    S = S + "        vpaddq  ymm1, ymm2, ymmword ptr [rdi + 544]\n"
    S = S + "        vpsrlq  ymm2, ymm1, 26\n"
    S = S + "        vpand   ymm1, ymm1, ymm0\n"
    S = S + "        vmovdqa ymmword ptr [rdi + 544], ymm1\n"
    S = S + "        vpaddq  ymm1, ymm2, ymmword ptr [rdi + 576]\n"
    S = S + "        vpsrlq  ymm2, ymm1, 26\n"
    S = S + "        vpand   ymm0, ymm1, ymm0\n"
    S = S + "        vmovdqa ymmword ptr [rdi + 576], ymm0\n"
    S = S + "        vpaddq  ymm0, ymm2, ymmword ptr [rdi + 608]\n"
    S = S + "        vmovdqa ymmword ptr [rdi + 608], ymm0\n"
    S = S + "        ret\n"     
    print(S)

    return 0    

def PrintMult(plimbs):


    # registers reserved rdi, rsi, rdx
    # rax, rbx = rcx, r8
    registers = ["ymm0", "ymm1", "ymm2", "ymm3", "ymm4", "ymm5", "ymm6", "ymm7", "ymm8", "ymm9", "ymm10", "ymm11", "ymm12", "ymm13"]
    reg_locked = [0] * 14

    # ymm14 = temp register t0
    # ymm15 = current b

    if(plimbs > len(registers)):
        print("ERROR: Index out range")
        exit()
    state = registers[:plimbs]
    #state = registers

    S = ""
    # S = ".intel_syntax noprefix\n\n"
    # S = S + ".section .rodata\n\n"
    # S = S + ".section .text\n\n"

    S = S + ".global mult_avx2_"+ str(plimbs) + "x" + str(plimbs) + "\n"
    S = S + "mult_avx2_"+ str(plimbs) + "x" + str(plimbs) + ":\n"
    
    # load as much a's as possible
    countAs = len(registers) - plimbs
    for j in range(0, countAs):
        S = S + "    vmovdqa " + registers[j] + ", YMMWORD PTR [rsi + " + str(j*32) + "] \n"
        reg_locked[j] = 1

    # mult bi x a0...ai
    for j in range(0,plimbs):
        # print(j)
        # print(reg_locked)
        # S = S + "    vmovdqu " + str(j*32) + "(%rdx), % \n"
        S = S + "    vmovdqa ymm15, YMMWORD PTR [rdx + " + str(j*32) + "] \n"        
        #   mults
        for k in range(0,plimbs):
            # S = S + "    vpmuldq %ymm15, %" + registers[k] + ", %" + registers[len(registers)-1 - ((j+k)%plimbs)] + " \n"
            if(j>0):
                if(reg_locked[len(registers)-1 - ((j+k)%plimbs)] == 1):
                    if(k >= countAs):
                        # read from stack
                        # S = S + "    vpmuldq " +  str(k*32) + "(%rsi), %ymm15, %ymm14 \n"
                        S = S + "    vpmuldq ymm14, ymm15, YMMWORD PTR [rsi + " +  str(k*32) + "]\n"
                    else:
                        #S = S + "    vpmuldq %ymm15, %" + registers[k] + ", %ymm14 \n"
                        S = S + "    vpmuldq ymm14, " + registers[k] + ", ymm15 \n"
                    if(0 in reg_locked):
                        S = S + "    vpaddq  " + registers[len(registers)-1 - ((j+k)%plimbs)] + ", ymm14, " + registers[len(registers)-1 - ((j+k)%plimbs)] + " \n"
                        reg_locked[len(registers)-1 - ((j+k)%plimbs)] = 1
                    else:
                        S = S + "################\n EXTRA LOAD 1\n"
                else:
                    if(k >= countAs):
                        # read from stack
                        S = S + "    vpmuldq " + registers[len(registers)-1 - ((j+k)%plimbs)] + ", ymm15, YMMWORD PTR [rsi + "  + str(k*32) + "]\n"
                        reg_locked[len(registers)-1 - ((j+k)%plimbs)] = 1
                    else:
                        if(0 in reg_locked):
                            S = S + "    vpmuldq " + registers[len(registers)-1 - ((j+k)%plimbs)] + ", ymm15, " + registers[k] + "\n"
                            reg_locked[len(registers)-1 - ((j+k)%plimbs)] = 1
                        else:
                            S = S + "################\n EXTRA LOAD 2\n"                    
                
            else:
                if(0 in reg_locked):
                    if(k >= countAs):
                        # read from stack
                        S = S + "    vpmuldq " + registers[len(registers)-1 - ((j+k)%plimbs)] + ", ymm15, YMMWORD PTR [rsi + " +  str(k*32) + "]\n"
                    else:
                        S = S + "    vpmuldq " + registers[len(registers)-1 - ((j+k)%plimbs)] + " , ymm15, " + registers[k] + "\n"
                    reg_locked[len(registers)-1 - ((j+k)%plimbs)] = 1
                else:
                    S = S + "################\n EXTRA LOAD 3\n"   

        S = S + "    vmovdqu YMMWORD PTR [rdi + " + str(j*32) + "] , " + registers[len(registers)-1 - j] + "\n"
        # S = S + "    vpxor %" + registers[len(registers)-1 - j] + ", %" + registers[len(registers)-1 - j] + " , %" + registers[len(registers)-1 - j] + " \n"        
        reg_locked[len(registers)-1 - j] = 0
        # S = S + "    vpxor %" + registers[len(registers)-1 - j] + ", %" + registers[len(registers)-1 - j] + " , %" + registers[len(registers)-1 - j] + " \n"

        # mult bi x a0...ai
    for j in range(0,plimbs-1):
        S = S + "    vmovdqu YMMWORD PTR [rdi + " + str((plimbs + j)*32) + "], " + registers[len(registers)- j - 1] + "\n"

    S = S + "    ret\n"

    print(S)


    return 0

def PrintAdd(plimbs):

    S = ""
    # S = ".intel_syntax noprefix\n\n"
    # S = S + ".section .rodata\n\n"
    # S = S + ".section .text\n\n"
    S = S + ".global add_"+ str(plimbs) + "x" + str(plimbs) + "\n"

    S = S + "add_"+ str(plimbs) + "x" + str(plimbs) + ":\n"
    S = S + "# intro\n"
    S = S + "    mov rax, [rsi + 0]\n    add rax, [rdx + 0]\n"
    S = S + "    mov [rdi + 0], rax\n"
    S = S + "# loop\n"
    S = S + "    .set k, 1\n"

    S = S + "    .rept " + str(plimbs-1) + "\n"

    S = S + "        mov rax, [rsi + 8*k]\n        adc rax, [rdx + 8*k]\n"
    S = S + "        mov [rdi + 8*k], rax\n        .set k, k+1\n    .endr\n"
    S = S + "# outro\n"
    S = S + "    mov rax, [rdi + " + str(plimbs) + "*8]\n"
    S = S + "    adc rax, 0\n"
    S = S + "    mov [rdi + " + str(plimbs) + "*8], rax\n"
    # S = S + "    setc al\n    movzx rax, al\n    ret\n\n"
    S = S + "    ret\n"

    print(S)
    return 0

#
# add function for "oddly" combinations like ADD_17x16, ADD_9x8
#
def PrintAddOdd(plimbs):

    S = ""
    # S = ".intel_syntax noprefix\n\n"
    # S = S + ".section .rodata\n\n"
    # S = S + ".section .text\n\n"
    S = S + ".global add_"+ str(plimbs + 1) + "x" + str(plimbs) + "\n"

    S = S + "add_"+ str(plimbs + 1) + "x" + str(plimbs) + ":\n"
    S = S + "# intro\n"
    S = S + "    mov rax, [rsi + 0]\n    add rax, [rdx + 0]\n"
    S = S + "    mov [rdi + 0], rax\n"
    S = S + "# loop\n"
    S = S + "    .set k, 1\n"

    S = S + "    .rept " + str(plimbs-1) + "\n"

    S = S + "        mov rax, [rsi + 8*k]\n        adc rax, [rdx + 8*k]\n"
    S = S + "        mov [rdi + 8*k], rax\n        .set k, k+1\n    .endr\n"
    S = S + "# outro\n"
    S = S + "    mov rax, [rsi + 8*" + str(plimbs) + "]\n"
    S = S + "    adc rax, 0\n"   
    S = S + "    mov [rdi + 8*" + str(plimbs) + "], rax\n"
    # S = S + "    setc al\n    movzx rax, al\n    ret\n\n"
    S = S + "    ret\n"


    print(S)
    return 0

# def PrintSub(plimbs):

#     registers = ["rax", "rbx", "rcx", "r8", "r9", "r10", "r11", "r12", "r13", "r14", "r15"]
    
#     S = ""
#     # S = ".intel_syntax noprefix\n\n"
#     # S = S + ".section .rodata\n\n"
#     # S = S + ".section .text\n\n"
#     S = S + ".global sub_d_"+ str(plimbs + 2) + "x" + str(plimbs + 2) + "_woc\n"    
#     S = S + "sub_d_"+ str(plimbs + 2) + "x" + str(plimbs + 2) + "_woc:\n"

#     S = S + push()

#     plimbs += 2
#     limbs_counter = plimbs

#     reg_counter = 0
#     S = S + "# intro\n"
#     while(limbs_counter > 0) and reg_counter < len(registers):
#         S = S + "    mov " + registers[reg_counter] + ", [rdi +  " + str(plimbs - limbs_counter) + "*8]\n"
#         limbs_counter -= 1
#         reg_counter += 1
#     S = S + "# loop\n"
#     for i in range(0,math.ceil(plimbs/len(registers))):
#         if i > 0:
#             reg_counter = 0
#             S = S + "# -------------------\n"
#             while(limbs_counter > 0) and reg_counter < len(registers)-1:
#                 S = S + "    mov " + registers[reg_counter] + ", [rdi +  " + str(plimbs - limbs_counter) + "*8]\n"
#                 limbs_counter -= 1
#                 reg_counter += 1
#             registers = rotate(registers, 1)
#         else:
#             reg_counter -= 1
#         S = S + "# -------------------\n"
#         for j in range(0, reg_counter):
#             if(j == 0) and (i == 0):
#                 S = S + "    sub " + registers[j] + ", [rsi + " + str(j + (i*len(registers))) + "*8]\n"
#                 S = S + "    sbb " + registers[j+1] + ", 0\n" 
#                 S = S + "    sub " + registers[j] + ", [rdx + " + str(j + (i*len(registers))) + "*8]\n"
#             else:
#                 S = S + "    sbb " + registers[j] + ", [rsi + " + str(j + (i*(len(registers)-1))) + "*8]\n"
#                 S = S + "    sbb " + registers[j+1] + ", 0\n" 
#                 S = S + "    sub " + registers[j] + ", [rdx + " + str(j + (i*(len(registers)-1))) + "*8]\n"
#         S = S + "# ------------------\n"
#         for k in range(0, reg_counter):
#             S = S + "    mov [rdi + " + str(k + (i*(len(registers)-1))) + "*8], " + registers[k] + "\n"
    
#     # outro
#     S = S + "# outro\n"
#     S = S + "    mov [rdi + " + str(reg_counter + (i*(len(registers)-1))) + "*8], " + registers[reg_counter] + "\n"
    
#     S = S + pop()

#     S = S + "    ret\n"
    

    
#     print(S)

    

#     return 0


def PrintSubOdd(plimbs):

    
    registers = ["rax", "rbx", "rcx", "r8", "r9", "r10", "r11", "r12", "r13", "r14", "r15"]
    
    S = ""
    # S = ".intel_syntax noprefix\n\n"
    # S = S + ".section .rodata\n\n"
    # S = S + ".section .text\n\n"
    S = S + ".global sub_d_"+ str(plimbs + 2) + "x" + str(plimbs) + "_woc\n"    
    S = S + "sub_d_"+ str(plimbs + 2) + "x" + str(plimbs) + "_woc:\n"

    S = S + push()

    plimbs += 1
    limbs_counter = plimbs

    reg_counter = 0
    S = S + "# intro\n"
    while(limbs_counter > 0) and reg_counter < len(registers):
        S = S + "    mov " + registers[reg_counter] + ", [rdi + " + str(plimbs - limbs_counter) + "*8]\n"
        limbs_counter -= 1
        reg_counter += 1
    
    S = S + "# loop\n"
    for i in range(0,math.ceil(plimbs/(len(registers)-1))):
        if i > 0:
            reg_counter = 0
            S = S + "# ------------------\n"
            while(limbs_counter > 0) and reg_counter < len(registers)-1:
                S = S + "    mov " + registers[reg_counter] + ", [rdi + " + str(plimbs - limbs_counter) + "*8]\n"
                limbs_counter -= 1
                reg_counter += 1
            registers = rotate(registers, 1)
        else:
            reg_counter -= 1
        S = S + "# ------------------\n"
        for j in range(0, reg_counter):
            if(j == 0) and (i == 0):
                S = S + "    sub " + registers[j] + ", [rsi + " + str(j + (i*len(registers))) + "*8]\n"
                S = S + "    sbb " + registers[j+1] + ", 0\n" 
                S = S + "    sub " + registers[j] + ", [rdx + " + str(j + (i*len(registers))) + "*8]\n"
            else:
                S = S + "    sbb " + registers[j] + ", [rsi + " + str(j + (i*(len(registers)-1))) + "*8]\n"
                S = S + "    sbb " + registers[j+1] + ", 0\n" 
                S = S + "    sub " + registers[j] + ", [rdx + " + str(j + (i*(len(registers)-1))) + "*8]\n"
        S = S + "# ------------------\n"
        for k in range(0, reg_counter):
            S = S + "    mov [rdi + " + str(k + (i*(len(registers)-1))) + "*8], " + registers[k] + "\n"
    
    # outro
    S = S + "# outro\n"
    S = S + "    sbb " + registers[reg_counter] + ", 0\n"
    S = S + "    mov [rdi + " + str(reg_counter + (i*(len(registers)-1))) + "*8], " + registers[reg_counter] + "\n"

    S = S + pop()

    S = S + "    ret\n"



    print(S)
    return 0

def PrintSub(plimbs):
    
    registers = ["rax", "rbx", "rcx", "r8", "r9", "r10", "r11", "r12", "r13", "r14", "r15"]
    
    S = ""
    # S = ".intel_syntax noprefix\n\n"
    # S = S + ".section .rodata\n\n"
    # S = S + ".section .text\n\n"
    S = S + ".global sub_d_"+ str(plimbs + 1) + "x" + str(plimbs + 1) + "_woc\n"    
    S = S + "sub_d_"+ str(plimbs + 1) + "x" + str(plimbs + 1) + "_woc:\n"

    S = S + push()

    plimbs += 1
    limbs_counter = plimbs

    reg_counter = 0
    S = S + "# intro\n"
    while(limbs_counter > 0) and reg_counter < len(registers):
        S = S + "    mov " + registers[reg_counter] + ", [rdi + " + str(plimbs - limbs_counter) + "*8]\n"
        limbs_counter -= 1
        reg_counter += 1
    
    S = S + "# loop\n"
    for i in range(0,((plimbs-1)//(len(registers)-1))):    
    # for i in range(0,math.ceil(plimbs/(len(registers)-1))-1):
        if i > 0:
            reg_counter = 0
            S = S + "# ------------------ " + str(i) + "\n"
            while(limbs_counter > 0) and reg_counter < len(registers)-1:
                S = S + "    mov " + registers[reg_counter] + ", [rdi + " + str(plimbs - limbs_counter) + "*8]\n"
                limbs_counter -= 1
                reg_counter += 1
            registers = rotate(registers, 1)
        else:
            reg_counter -= 1
        S = S + "# ------------------\n"
        for j in range(0, reg_counter):
            if(j == 0) and (i == 0):
                S = S + "    sub " + registers[j] + ", [rsi + " + str(j + (i*len(registers))) + "*8]\n"
                S = S + "    sbb " + registers[j+1] + ", 0\n" 
                S = S + "    sub " + registers[j] + ", [rdx + " + str(j + (i*len(registers))) + "*8]\n"
            else:
                S = S + "    sbb " + registers[j] + ", [rsi + " + str(j + (i*(len(registers)-1))) + "*8]\n"
                S = S + "    sbb " + registers[j+1] + ", 0\n" 
                S = S + "    sub " + registers[j] + ", [rdx + " + str(j + (i*(len(registers)-1))) + "*8]\n"
        S = S + "# ------------------\n"
        for k in range(0, reg_counter):
            S = S + "    mov [rdi + " + str(k + (i*(len(registers)-1))) + "*8], " + registers[k] + "\n"

    S = S + "# last loop\n"
    if(1):
        i = (plimbs-1)//(len(registers)-1)
        # i = math.ceil(plimbs/(len(registers)-1)) -1        
        if i > 0:
            reg_counter = 0
            S = S + "# ------------------\n"
            while(limbs_counter > 0) and reg_counter < len(registers)-1:
                S = S + "    mov " + registers[reg_counter] + ", [rdi + " + str(plimbs - limbs_counter) + "*8]\n"
                limbs_counter -= 1
                reg_counter += 1
            registers = rotate(registers, 1)
        else:
            reg_counter -= 1
        S = S + "# ------------------\n"
        for j in range(0, reg_counter - 1):
            if(j == 0) and (i == 0):
                S = S + "    sub " + registers[j] + ", [rsi + " + str(j + (i*len(registers))) + "*8]\n"
                S = S + "    sbb " + registers[j+1] + ", 0\n" 
                S = S + "    sub " + registers[j] + ", [rdx + " + str(j + (i*len(registers))) + "*8]\n"
            else:
                S = S + "    sbb " + registers[j] + ", [rsi + " + str(j + (i*(len(registers)-1))) + "*8]\n"
                S = S + "    sbb " + registers[j+1] + ", 0\n" 
                S = S + "    sub " + registers[j] + ", [rdx + " + str(j + (i*(len(registers)-1))) + "*8]\n"
        S = S + "# ------------------\n"
        for k in range(0, reg_counter):
            S = S + "    mov [rdi + " + str(k + (i*(len(registers)-1))) + "*8], " + registers[k] + "\n"       
    
    
    # outro
    S = S + "# outro\n"
    S = S + "    sbb " + registers[reg_counter - 1] + ", 0\n"
    S = S + "    sub " + registers[reg_counter - 1] + ", [rsi + " + str(reg_counter - 1 + (i*(len(registers)-1))) + "*8]\n"
    S = S + "    sbb " + registers[reg_counter] + ", 0\n"
    S = S + "    sub " + registers[reg_counter] + ", [rsi + " + str(reg_counter + (i*(len(registers)-1))) + "*8]\n"    
    S = S + "    mov [rdi + " + str(reg_counter - 1 + (i*(len(registers)-1))) + "*8], " + registers[reg_counter - 1] + "\n"
    S = S + "    mov [rdi + " + str(reg_counter + (i*(len(registers)-1))) + "*8], " + registers[reg_counter] + "\n"

    S = S + pop()

    S = S + "    ret\n"

    print(S)
    return 0    

#////////////////////////////////////////
def Print_Assembly(plimbs):
    global sefOfLimbs
    global printOut
    breakOut = 5
    if(printOut):
        while 1 :
            if plimbs > breakOut :
                print("++++++++++++++++++++++++++")
                if(plimbs not in sefOfLimbs):
                    sefOfLimbs.add(plimbs)
                    print(str(plimbs) + "x" + str(plimbs))
                    if(plimbs%2!=1):
                        print("add_" + str(plimbs//2) + "x" + str(plimbs//2))
                        print("sub_d_" + str(plimbs + 2) + "x" + str(plimbs) + "_woc")
                        print("add_" + str(plimbs + 2) + "x" + str(plimbs + 1))
                        if(plimbs > breakOut):
                        #     print("mult_"+ str(plimbs//2) + "x" + str(plimbs//2))
                        #     print("mult_"+ str(plimbs//2+1) + "x" + str(plimbs//2+1))
                        # else:
                            Print_Assembly(plimbs//2)
                            Print_Assembly(plimbs//2+1)
                    else:
                        print("add_"+ str(plimbs//2 + 1) + "x" + str(plimbs//2))            
                        print("sub_d_"+ str(plimbs + 1) + "x" + str(plimbs + 1) + "_woc")
                        print("add_"+ str(plimbs + 2) + "x" + str(plimbs + 1))
                        if(plimbs > breakOut):
                        #     print("mult_"+ str(plimbs//2) + "x" + str(plimbs//2))
                        #     print("mult_"+ str(plimbs//2+1) + "x" + str(plimbs//2+1))
                        # else:
                            Print_Assembly(plimbs//2)
                            Print_Assembly(plimbs//2+1)                    
                    # plimbs = plimbs // 2
                else:
                    plimbs = 0
                    break
            else:
                if plimbs <= breakOut:
                    if(plimbs not in sefOfLimbs):
                        sefOfLimbs.add(plimbs)
                        print("mult_"+ str(plimbs) + "x" + str(plimbs))
                    if(plimbs+1 not in sefOfLimbs):                    
                        if(plimbs+1 <= breakOut):
                            sefOfLimbs.add(plimbs+1)
                            print("mult_"+ str(plimbs+1) + "x" + str(plimbs+1))
                    # PrintMult(plimbs+1)
                break
    else:
        while 1 :
            if plimbs > breakOut :
                if(plimbs not in sefOfLimbs):
                    sefOfLimbs.add(plimbs)
                    print("# ------------------" + str(plimbs) + "x" + str(plimbs))
                    if(plimbs%2!=1):
                        PrintAdd(plimbs//2)
                        PrintSubOdd(plimbs)
                        PrintAddOdd(plimbs+1)
                        if(plimbs > breakOut):
                            Print_Assembly(plimbs//2)
                            Print_Assembly(plimbs//2+1)
                    else:
                        PrintAddOdd(plimbs//2)
                        PrintSub(plimbs)
                        PrintAddOdd(plimbs+1)
                        if(plimbs > breakOut):
                            Print_Assembly(plimbs//2)
                            Print_Assembly(plimbs//2+1)    
                else:
                    plimbs = 0
                    break
            else:
                if plimbs <= breakOut:
                    if(plimbs not in sefOfLimbs):
                        sefOfLimbs.add(plimbs)
                        PrintMult(plimbs)
                    if(plimbs+1 not in sefOfLimbs):                    
                        if(plimbs+1 <= breakOut):
                            sefOfLimbs.add(plimbs+1)                        
                            PrintMult(plimbs+1)
                break



#//+++++++++++++++ Main ++++++++++++++++++//

def main(argv):
    global printOut
    if(len(argv) != 2):
        print(
            "\nplease specify number of limbs and type of execution [1 = only printing functions out, 0 = print full assembly]\n")
        sys.exit()
    else:
        printOut = int(argv[1])

        # pbits = math.ceil(math.log(p, 2.0))
        # pbytes = math.ceil(pbits/8)
        # if((pbytes%4) != 0):
        #     pbytes = pbytes + 4 - (pbytes%4)
        # pwords = math.ceil(pbits/64)

        S = ".intel_syntax noprefix\n\n"
        S = S + ".section .rodata\n\n"
        S = S + ".section .text\n\n"

        print(S)

        # PrintMult(int(argv[0]))
        PrintMultInterleaved(int(argv[0]))
        # PrintSquaringInterleaved(int(argv[0]))

        # Print2xAddAVX2(2)
        # PrintSubAVX2(4)
        # PrintDeInterleave_4x_10x10()
        # Print_Assembly(int(argv[0]))


if __name__ == "__main__":
    main(sys.argv[1:])
