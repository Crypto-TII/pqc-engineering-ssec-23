#!/usr/bin/env sage -python

# rdi, rsi, rdx
# rax

# how to call
# sage -python AsmMultCodegenerator.py  > fp9216.s

from sage.all import *
import math

sefOfLimbs = set()
printOut = 0

def rotate(l, x):
    return l[-x:] + l[:-x]

def push():
    # S = "# -------------------\n"
    S = "# push\n"
    # S = S + "    push rbx\n    push rbp\n    push edi\n    push esi\n    push r12d\n    push r13d\n    push r14d\n    push r15d\n\n"
    S = S + "    push rbx\n    push rbp\n    push rsi\n    push r12\n    push r13\n    push r14\n    push r15\n\n"    
    # S = S + "    push rdx\n    push edi\n    push esi\n\n"    
    return S

def pop():
    # S = "# -------------------\n"
    S = "# pop\n"    
    # S = S + "    pop r15d\n    pop r14d\n    pop r13d\n    pop r12d\n    pop esi\n    pop edi\n    pop rbp\n    pop rbx\n\n"
    S = S + "    pop r15\n    pop r14\n    pop r13\n    pop r12\n    pop rsi\n    pop rbp\n    pop rbx\n\n"    
    # S = S + "    pop esi\n    pop edi\n    pop rdx\n\n"    
    return S

def OneTimeCarry(plimbs):
    # registers reserved edi, esi, rdx
    # eax, rbx = ecx, r8d
    registers = ["r8d", "r9d", "r10d", "r11d", "r12d", "r13d", "r14d", "r15d"]
    registers64 = ["r8", "r9", "r10", "r11", "r12", "r13", "r14", "r15"]

    # if(plimbs > len(registers)):
    #     print("ERROR: Index out range")
    #     exit()
    state = registers[:plimbs]
    state64 = registers64[:plimbs]

    S = ""
    # S = ".intel_syntax noprefix\n\n"
    # S = S + ".section .rodata\n\n"
    # S = S + ".section .text\n\n"

    # S = S + ".macro p_times_w\n"
    # S = S + "mult_"+ str(plimbs) + "x" + str(plimbs) + ":\n"

    S = S + ".global secsidh_internal_2047k221_OneTimeCarry\nsecsidh_internal_2047k221_OneTimeCarry:\n"
    
    S = S + push()

    for j in range(0,plimbs):
        S = S + "    vmovdqa ymm15, YMMWORD PTR [rsi + " + str(j) + "*32]\n"
        S = S + "    vpbroadcastq ymm0, qword ptr [rip + .base]\n"
        S = S + "    vpand ymm1, ymm15, ymm0\n"
        S = S + "    vpandn ymm15, ymm0, ymm15\n"
        S = S + "    vpsrlq ymm15, ymm15, 26\n"
        S = S + "    vpaddq ymm15, ymm15, ymm1\n"
        S = S + "    vmovdqa YMMWORD PTR [rdi + " + str(j) + "*32], ymm15\n"        
    S = S + pop()

    S = S + "   ret\n"    
    return S


def MontRedcAdd(plimbs):
    # registers reserved edi, esi, rdx
    # eax, rbx = ecx, r8d
    registers = ["r8d", "r9d", "r10d", "r11d", "r12d", "r13d", "r14d", "r15d"]
    registers64 = ["r8", "r9", "r10", "r11", "r12", "r13", "r14", "r15"]

    # if(plimbs > len(registers)):
    #     print("ERROR: Index out range")
    #     exit()
    state = registers[:plimbs]
    state64 = registers64[:plimbs]

    S = ""
    # S = ".intel_syntax noprefix\n\n"
    # S = S + ".section .rodata\n\n"
    # S = S + ".section .text\n\n"

    # S = S + ".macro p_times_w\n"
    # S = S + "mult_"+ str(plimbs) + "x" + str(plimbs) + ":\n"

    S = S + ".global secsidh_internal_2047k221_a_plus_u_i\secsidh_internal_2047k221_a_plus_u_i:\n"
    
    S = S + push()

    for j in range(0,plimbs):
        S = S + "    vmovdqu ymm15, YMMWORD PTR [rdi + rdx*4 + " + str(j) + "*32]\n"
        S = S + "    vpaddd ymm15 , ymm15, ymmword ptr [rsi + " + str(j) + "*32]\n"
        S = S + "    vmovdqu YMMWORD PTR [rdi + rdx*4 + " + str(j) + "*32], ymm15\n"        
    S = S + pop()

    S = S + "   ret\n"    
    return S

# def fp_copy(plimbs):
#     # registers reserved edi, esi, rdx
#     # eax, rbx = ecx, r8d
#     registers = ["r8d", "r9d", "r10d", "r11d", "r12d", "r13d", "r14d", "r15d"]
#     registers64 = ["r8", "r9", "r10", "r11", "r12", "r13", "r14", "r15"]

#     # if(plimbs > len(registers)):
#     #     print("ERROR: Index out range")
#     #     exit()
#     state = registers[:plimbs]
#     state64 = registers64[:plimbs]

#     S = ""
#     # S = ".intel_syntax noprefix\n\n"
#     # S = S + ".section .rodata\n\n"
#     # S = S + ".section .text\n\n"

#     # S = S + ".macro p_times_w\n"
#     # S = S + "mult_"+ str(plimbs) + "x" + str(plimbs) + ":\n"

#     S = S + ".global secsidh_internal_2047k221_a_plus_u_i\secsidh_internal_2047k221_a_plus_u_i:\n"
    
#     S = S + push()

#     S = S + "    xor r8, r8\n"
#     S = S + "    xor r9, r9\n"
#     for j in range(0,plimbs):
#         S = S + "    vmovdqa ymm15, YMMWORD PTR [rdi + " + str(j) + "*32]\n"
#         S = S + "    vptest r8 , ymm15, ymm15\n"
#         S = S + "     r9 , ymm2, ymm1\n"
#     S = S + "    vmovdqa YMMWORD PTR [rdi + " + str(j) + "*32], ymm15\n"        
#     S = S + pop()

#     S = S + "   ret\n"    
#     return S    
    

def fp_copy(plimbs):
    # registers reserved edi, esi, rdx
    # eax, rbx = ecx, r8d
    registers = ["r8d", "r9d", "r10d", "r11d", "r12d", "r13d", "r14d", "r15d"]
    registers64 = ["r8", "r9", "r10", "r11", "r12", "r13", "r14", "r15"]

    # if(plimbs > len(registers)):
    #     print("ERROR: Index out range")
    #     exit()
    state = registers[:plimbs]
    state64 = registers64[:plimbs]

    S = ""
    # S = ".intel_syntax noprefix\n\n"
    # S = S + ".section .rodata\n\n"
    # S = S + ".section .text\n\n"

    # S = S + ".macro p_times_w\n"
    # S = S + "mult_"+ str(plimbs) + "x" + str(plimbs) + ":\n"

    S = S + ".global secsidh_internal_2047k221_fp_copy\secsidh_internal_2047k221_fp_copy:\n"
    
    S = S + push()

    for j in range(0,plimbs):
        S = S + "    vmovdqa ymm15, YMMWORD PTR [rsi + " + str(j) + "*32]\n"
        S = S + "    vmovdqa YMMWORD PTR [rdi + " + str(j) + "*32], ymm15\n"
    S = S + pop()

    S = S + "   ret\n"    
    return S    


def PrintMult(plimbs):
    

    # registers reserved edi, esi, rdx
    # eax, rbx = ecx, r8d
    registers = ["r8d", "r9d", "r10d", "r11d", "r12d", "r13d", "r14d", "r15d"]
    registers64 = ["r8", "r9", "r10", "r11", "r12", "r13", "r14", "r15"]

    # if(plimbs > len(registers)):
    #     print("ERROR: Index out range")
    #     exit()
    state = registers[:plimbs]
    state64 = registers64[:plimbs]

    S = ""
    # S = ".intel_syntax noprefix\n\n"
    # S = S + ".section .rodata\n\n"
    # S = S + ".section .text\n\n"

    # S = S + ".macro p_times_w\n"
    # S = S + "mult_"+ str(plimbs) + "x" + str(plimbs) + ":\n"

    S = S + ".global secsidh_internal_2047k221_p_times_w\nsecsidh_internal_2047k221_p_times_w:\n"
    
    S = S + push()

    
    
    for j in range(0,plimbs):
        # S = S + "    vmovd xmm0, edx\n"
        # S = S + "    vpbroadcastd ymm0, xmm0\n" 
        S = S + "    vmovdqu ymm2, ymmword ptr [rdx + " + str(j) + "*4]\n"
        S = S + "    vpbroadcastd ymm0, xmm2\n"
        S = S + "    vmovdqa ymm1, ymmword ptr [rip + .SHUFFLE_MUL]\n"
        S = S + "#############################\n"
        S = S + "    vmovdqa ymm15, YMMWORD PTR [esi + " + str(j) + "*32]\n"
        S = S + "    vpermd ymm15, ymm1, ymm15\n"
        S = S + "    vpmuldq ymm11 , ymm15, ymm0\n"
        S = S + "    vmovdqa ymmword ptr [rdi + " + str(2*j) + "*32] , ymm11\n"
        S = S + "    vpshufd ymm15, ymm15, 177\n"
        S = S + "    vpmuldq ymm12 , ymm15, ymm0\n"
        S = S + "    vmovdqa ymmword ptr [rdi + " + str(2*j+1) + "*32] , ymm12\n"       
        S = S + "#############################\n"
    S = S + pop()

    S = S + "   ret\n"    
    return S


def fp_cmov(plimbs):
    

    # registers reserved edi, esi, rdx
    # eax, rbx = ecx, r8d
    registers = ["r8d", "r9d", "r10d", "r11d", "r12d", "r13d", "r14d", "r15d"]
    registers64 = ["r8", "r9", "r10", "r11", "r12", "r13", "r14", "r15"]

    # if(plimbs > len(registers)):
    #     print("ERROR: Index out range")
    #     exit()
    state = registers[:plimbs]
    state64 = registers64[:plimbs]

    S = ""
    # S = ".intel_syntax noprefix\n\n"
    # S = S + ".section .rodata\n\n"
    # S = S + ".section .text\n\n"

    # S = S + ".macro p_times_w\n"
    # S = S + "mult_"+ str(plimbs) + "x" + str(plimbs) + ":\n"

    S = S + ".global fp_cmov\nfp_cmov:\n"

    S = S + "    mov     eax, edx\n"
    S = S + "    neg     rax\n" 
    S = S + "    vmovq   xmm0, rax\n"     
    S = S + "    vpbroadcastq    ymm0, xmm0\n"    
    S = S + "#############################\n"
    for j in range(0,plimbs):

        S = S + "    vpandn ymm1, ymm0, ymmword ptr [rdi + " + str(j) + "*32]\n"
        S = S + "    vpand ymm2, ymm0, ymmword ptr [rsi + " + str(j) + "*32]\n"
        S = S + "    vpor    ymm1, ymm2, ymm1\n"
        S = S + "    vmovdqa ymmword ptr [rdi + " + str(j) + "*32], ymm1\n"


    S = S + "    vzeroupper\n"
    S = S + "   ret\n"    
    return S


#//+++++++++++++++ Main ++++++++++++++++++//

def main(argv):
    global printOut
    if(len(argv) < 2):
        print(
            "\nplease specify the prime and type of execution [1 = only printing functions out, 0 = print full assembly]\n")
        sys.exit()
    else:
        printOut = int(argv[1])
        prime = argv[0]
        length = 0

        if(prime=="2047k221"):
            length = 2048

        elif(prime=="4095k256"):
            length = 4096

        elif(prime=="5119k234"):
            length = 5120
  
        elif(prime=="6143k256"):
            length = 6144
                             
        elif(prime=="8191k332"):
            length = 8192

        elif(prime=="9215k384"):
            length = 9216
                               
        else:
            print("\nError : no prime available for this input\n")
            sys.exit()                 


        S = ".intel_syntax noprefix\n\n"
        S = S + ".section .rodata\n\n"
        S = S + ".section .text\n\n"
        

        print(S)

        # plimbs = 32
        plimbs = 10

        # S = PrintMult(plimbs)


        # S = ".global fp_mult_"+ str(plimbs) + "x" + str(plimbs) + "\n"
        # S = S + "fp_mult_"+ str(plimbs) + "x" + str(plimbs) + ":\n"
        # S = S + "    mult_"+ str(plimbs) + "x" + str(plimbs) + "\n"
        # S = S + "    ret\n"

        # print(S)

        S = MontRedcAdd(plimbs)

        print(S)

        # S = fp_copy(plimbs)

        # print(S)

        # S = OneTimeCarry(plimbs)

        # print(S)

        # S = fp_cmov(plimbs)

        # print(S)

        print("\n")

if __name__ == "__main__":
    main(sys.argv[1:])
