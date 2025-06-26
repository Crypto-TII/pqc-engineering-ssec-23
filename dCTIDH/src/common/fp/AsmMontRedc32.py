#!/usr/bin/env sage -python


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

def MontRedcAdd(plimbs):
    

    # registers reserved edi, esi, rdx
    # eax, rbx = ecx, r8d
    registers = ["r8d", "r9d", "r10d", "r11d", "r12d", "r13d", "r14d", "r15d"]
    registers64 = ["r8", "r9", "r10", "r11", "r12", "r13", "r14", "r15"]

    state = registers[:plimbs]
    state64 = registers64[:plimbs]
    #state = registers

    S = ""
    # S = ".intel_syntax noprefix\n\n"
    # S = S + ".section .rodata\n\n"
    # S = S + ".section .text\n\n"

    # S = S + ".macro p_times_w\n"
    # S = S + "mult_"+ str(plimbs) + "x" + str(plimbs) + ":\n"

    S = S + ".global secsidh_internal_2047k221_a_plus_u_i\nsecsidh_internal_2047k221_a_plus_u_i:\n"

    S = S + push()    

    S = S + "   lea rdx, [rdi + 0]\n"
# intro
    S = S + "   mov rax, [rsi + 0]\n"
    S = S + "   add eax, [edx + 0]\n"
    S = S + "   mov [rdi + 0], rax\n"
# loop
    S = S + "   .set k, 1\n"
    S = S + "   .rept " + str(plimbs) + "\n"
    S = S + "       mov rax, [rsi + 4*k]\n"
    S = S + "       adc eax, [edx + 4*k]\n"
    S = S + "       mov [rdi + 4*k], rax\n"
    S = S + "       .set k, k+1\n"
    S = S + "   .endr\n"
# outro
    S = S + "   mov rax, [rdi + 4*" + str(plimbs+1) + "]\n"
    S = S + "   adc eax, 0\n"
    S = S + "   mov [rdi + 4*" + str(plimbs+1) + "], rax\n"
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
    
    S = S + "    mov r15d, edx\n"
    S = S + "    mov eax, r15d\n"
    S = S + "    mov " + state[0] + ", [esi + 0*4]\n"    
    S = S + "    imul " + state[0] + "\n"
    S = S + "    mov rbx, rax\n"
    S = S + "    and  ebx, 67108863\n"
    S = S + "    mov  [rdi + 0*4], rbx\n"
    S = S + "    shr  rax, 26\n";
    S = S + "    add rdx, rax\n"    
    S = S + "    mov " + state64[0] + ", rdx\n"
    S = S + "#############################\n"
    for j in range(1,plimbs):
        if(j == plimbs-1):
            S = S + "    mov " + state[j%2] + ", [esi + " + str(j) + "*4]\n"
            S = S + "    mov eax, r15d\n"
            S = S + "    imul " + state[j%2] + "\n"
            S = S + "    mov rbx, rax\n"
            S = S + "    add  ebx, " + state[(j+1)%2] + "\n";              
            S = S + "    and  ebx, 67108863\n"          
            S = S + "    mov  [rdi + " + str(j) + "*4], rbx\n"
            S = S + "    shr  rax, 26\n";
            S = S + "    add rdx, rax\n" 
            S = S + "    mov " + state64[j%2] + ", rdx\n"          
            S = S + "    adc " + state[j%2] + ", 0\n"
            S = S + "    mov [rdi + " + str(j) + "*4], "  + state64[(j+1)%2] + "\n"
            S = S + "    mov [rdi + " + str(j+1) + "*4], "  + state64[j%2] + "\n"                        
            S = S + "#############################\n"
        elif(j == 1):
            S = S + "    mov " + state[j%2] + ", [esi + " + str(j) + "*4]\n"
            S = S + "    mov eax, r15d\n"
            S = S + "    imul " + state[j%2] + "\n"
            S = S + "    mov rbx, rax\n"
            S = S + "    add  ebx, " + state[(j+1)%2] + "\n";
            S = S + "    and  ebx, 67108863\n"
            S = S + "    mov  [rdi + " + str(j) + "*4], rbx\n"
            S = S + "    shr  rax, 26\n";
            S = S + "    add rdx, rax\n" 
            S = S + "    mov " + state64[j%2] + ", rdx\n"            
            S = S + "#############################\n"
        else:
            S = S + "    mov " + state[j%2] + ", [esi + " + str(j) + "*4]\n"
            S = S + "    mov eax, r15d\n"
            S = S + "    imul " + state[j%2] + "\n"
            S = S + "    mov rbx, rax\n"
            S = S + "    add  ebx, " + state[(j+1)%2] + "\n";    
            S = S + "    and  ebx, 67108863\n" 
            S = S + "    mov  [rdi + " + str(j) + "*4], rbx\n"
            S = S + "    shr  rax, 26\n";
            S = S + "    add rdx, rax\n" 
            S = S + "    mov " + state64[j%2] + ", rdx\n" 
            S = S + "#############################\n"        
    
    S = S + pop()

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

        plimbs = length//26
        plimbs = 80

        S = PrintMult(plimbs)


        # S = ".global fp_mult_"+ str(plimbs) + "x" + str(plimbs) + "\n"
        # S = S + "fp_mult_"+ str(plimbs) + "x" + str(plimbs) + ":\n"
        # S = S + "    mult_"+ str(plimbs) + "x" + str(plimbs) + "\n"
        # S = S + "    ret\n"

        print(S)

        S = MontRedcAdd(plimbs)

        print(S)

        print("\n")

if __name__ == "__main__":
    main(sys.argv[1:])
