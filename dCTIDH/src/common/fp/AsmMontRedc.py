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
    # S = S + "    push rbx\n    push rbp\n    push rdi\n    push rsi\n    push r12\n    push r13\n    push r14\n    push r15\n\n"
    S = S + "    push rbx\n    push rbp\n    push rsi\n    push r12\n    push r13\n    push r14\n    push r15\n\n"    
    # S = S + "    push rdx\n    push rdi\n    push rsi\n\n"    
    return S

def pop():
    # S = "# -------------------\n"
    S = "# pop\n"    
    # S = S + "    pop r15\n    pop r14\n    pop r13\n    pop r12\n    pop rsi\n    pop rdi\n    pop rbp\n    pop rbx\n\n"
    S = S + "    pop r15\n    pop r14\n    pop r13\n    pop r12\n    pop rsi\n    pop rbp\n    pop rbx\n\n"    
    # S = S + "    pop rsi\n    pop rdi\n    pop rdx\n\n"    
    return S


def MontRedcAdd(plimbs):
    

    # registers reserved rdi, rsi, rdx
    # rax, rbx = rcx, r8
    registers = ["r8", "r9", "r10", "r11", "r12", "r13", "r14", "r15"]
    # if(plimbs > len(registers)):
    #     print("ERROR: Index out range")
    #     exit()
    state = registers[:plimbs]
    #state = registers

    S = ""
    # S = ".intel_syntax noprefix\n\n"
    # S = S + ".section .rodata\n\n"
    # S = S + ".section .text\n\n"

    # S = S + ".macro p_times_w\n"
    # S = S + "mult_"+ str(plimbs) + "x" + str(plimbs) + ":\n"

    S = S + ".global u_i_times_p\nu_i_times_p:\n"

    S = S + push()    

    S = S + "   lea rdx, [rdi + 0]  ]\n"
# intro
    S = S + "   mov rax, [rsi + 0]\n"
    S = S + "   add rax, [rdx + 0]\n"
    S = S + "   mov [rdi + 0], rax\n"
# loop
    S = S + "   .set k, 1\n"
    S = S + "   .rept " + str(plimbs) + "\n"
    S = S + "       mov rax, [rsi + 8*k]\n"
    S = S + "       adc rax, [rdx + 8*k]\n"
    S = S + "       mov [rdi + 8*k], rax\n"
    S = S + "       .set k, k+1\n"
    S = S + "   .endr\n"
# outro
    S = S + "   mov rax, [rdi + 8*" + str(plimbs+1) + "]\n"
    S = S + "   adc rax, 0\n"
    S = S + "   mov [rdi + 8*" + str(plimbs+1) + "], rax\n"
    S = S + pop()
    S = S + "   ret\n"

    return S


def MontRedcAdd(plimbs):
    

    # registers reserved rdi, rsi, rdx
    # rax, rbx = rcx, r8
    registers = ["r8", "r9", "r10", "r11", "r12", "r13", "r14", "r15"]
    # if(plimbs > len(registers)):
    #     print("ERROR: Index out range")
    #     exit()
    state = registers[:plimbs]
    #state = registers

    S = ""
    # S = ".intel_syntax noprefix\n\n"
    # S = S + ".section .rodata\n\n"
    # S = S + ".section .text\n\n"

    # S = S + ".macro p_times_w\n"
    # S = S + "mult_"+ str(plimbs) + "x" + str(plimbs) + ":\n"

    S = S + ".global a_plus_u_i\na_plus_u_i:\n"

    S = S + push()    

    S = S + "   lea rdx, [rdi + 0]  ]\n"
# intro
    S = S + "   mov rax, [rsi + 0]\n"
    S = S + "   add rax, [rdx + 0]\n"
    S = S + "   mov [rdi + 0], rax\n"
# loop
    S = S + "   .set k, 1\n"
    S = S + "   .rept " + str(plimbs) + "\n"
    S = S + "       mov rax, [rsi + 8*k]\n"
    S = S + "       adc rax, [rdx + 8*k]\n"
    S = S + "       mov [rdi + 8*k], rax\n"
    S = S + "       .set k, k+1\n"
    S = S + "   .endr\n"
# outro
    S = S + "   mov rax, [rdi + 8*" + str(plimbs+1) + "]\n"
    S = S + "   adc rax, 0\n"
    S = S + "   mov [rdi + 8*" + str(plimbs+1) + "], rax\n"
    S = S + pop()
    S = S + "   ret\n"

    return S

def PrintMontLoop(plimbs):

    S = ""

    # S = S + "push rbp\n"
    # S = S + "mov rbp, rsp\n"
    # # allocate stack
    # S = S + "sub rsp, " + str(plimbs*8) + "\n"
    S = S + "lea rcx, [rsi]\n"
    S = S + "mov r8, secsidh_internal_2047k221_p@GOTPCREL[rip]\n"

    
    for k in range(0,plimbs-1):    
        S = S + "##########################\n"
        S = S + "xor r11, r11\n\n"
        S = S + "mov rdx, [rcx + 8*" + str(k) + "]\n"

        for j in range(0,plimbs):
            # high, low, p[i], A[i]
            S = S + "mulx r9, r10, [r8 + 8*" + str(j) + "]\n"
            # add low to "high - 1"
            S = S + "adcx r10, r11\n"
            # save new high
            S = S + "mov r11, r9\n"
            # S = S + "mov [rdi + 8*" + str(j) + "], r10\n\n"
            #S = S + "mov [rsi + 8*" + str(j) + " ], r10\n\n"
            
            S = S + "adox r10, [rcx + 8*" + str(j) + " + 8*" + str(k) + "]\n"
            S = S + "mov  [rcx + 8*" + str(j) + " + 8*" + str(k) + "], r10\n\n"
            # S = S + "mov [rbp - 256 + 8*" + str(j) + " ], r10\n\n"

        S = S + "adox r11, [rcx + 8*" + str(plimbs) + " + 8*" + str(k) + "]\n"
        S = S + "mov [rcx + 8*" + str(plimbs) + " + 8*" + str(k) + "], r11\n"

    S = S + "##########################\n"
    S = S + "##########################\n"

    S = S + "xor r11, r11\n\n"
    S = S + "mov rdx, [rcx + 8*" + str(plimbs-1) + "]\n"
    # high, low, p[i], A[i]
    S = S + "mulx r9, r10, [r8]\n"
    # add low to "high - 1"
    S = S + "adcx r10, r11\n"
    # save new high
    S = S + "mov r11, r9\n"
    # S = S + "mov [rdi + 8*" + str(j) + "], r10\n\n"
    #S = S + "mov [rsi + 8*" + str(j) + " ], r10\n\n"
    S = S + "adox r10, [rcx + 8*" + str(plimbs-1) + "]\n"

    S = S + "##########################\n"
    
    for j in range(1,plimbs):
        # high, low, p[i], A[i]
        S = S + "mulx r9, r10, [r8 + 8*" + str(j) + "]\n"
        # add low to "high - 1"
        S = S + "adcx r10, r11\n"
        # save new high
        S = S + "mov r11, r9\n"
        # S = S + "mov [rdi + 8*" + str(j) + "], r10\n\n"
        #S = S + "mov [rsi + 8*" + str(j) + " ], r10\n\n"
        
        S = S + "adox r10, [rcx + 8*" + str(j) + " + 8*" + str(plimbs-1) + "]\n"
        S = S + "mov  [rdi + 8*" + str(j-1) + "], r10\n\n"
        # S = S + "mov [rbp - 256 + 8*" + str(j) + " ], r10\n\n"

    S = S + "adox r11, [rcx + 8*" + str(plimbs) + " + 8*" + str(plimbs-1) + "]\n"
    S = S + "mov [rdi + 8*" + str(plimbs-1) + "], r11\n"



    return S

def PrintMult(plimbs):
    

    # registers reserved rdi, rsi, rdx
    # rax, rbx = rcx, r8
    registers = ["r8", "r9", "r10", "r11", "r12", "r13", "r14", "r15"]
    # if(plimbs > len(registers)):
    #     print("ERROR: Index out range")
    #     exit()
    state = registers[:plimbs]
    #state = registers

    S = ""
    # S = ".intel_syntax noprefix\n\n"
    # S = S + ".section .rodata\n\n"
    # S = S + ".section .text\n\n"

    # S = S + ".macro p_times_w\n"
    # S = S + "mult_"+ str(plimbs) + "x" + str(plimbs) + ":\n"

    S = S + ".global p_times_w\np_times_w:\n"
    
    S = S + push()
    
    S = S + "    mulx " + state[0] + ", rcx, [rsi + 0*8]\n"
    S = S + "    mov [rdi + 0*8], rcx\n"
    for j in range(1,plimbs):
        if(j == plimbs-1):
            S = S + "    mulx " + state[j%2] + ", rax, [rsi + " + str(j) + "*8]\n"
            S = S + "    adc " + state[(j+1)%2] + ", rax\n"            
            S = S + "    adc " + state[j%2] + ", 0\n"
            S = S + "    mov [rdi + " + str(j) + "*8], "  + state[(j+1)%2] + "\n"
            S = S + "    mov [rdi + " + str(j+1) + "*8], "  + state[j%2] + "\n"                        
        elif(j == 1):
            S = S + "    mulx " + state[j%2] + ", rax, [rsi + " + str(j) + "*8]\n"
            S = S + "    add " + state[(j+1)%2] + ", rax\n"
            S = S + "    mov [rdi + " + str(j) + "*8], "  + state[(j+1)%2] + "\n"
        else:
            S = S + "    mulx " + state[j%2] + ", rax, [rsi + " + str(j) + "*8]\n"
            S = S + "    adc " + state[(j+1)%2] + ", rax\n"
            S = S + "    mov [rdi + " + str(j) + "*8], "  + state[(j+1)%2] + "\n"            
    
    S = S + pop()
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

        plimbs = length//64

        # S = PrintMult(plimbs)
        # S = MontRedcAdd(plimbs)
        S = PrintMontLoop(plimbs)

        # S = ".global fp_mult_"+ str(plimbs) + "x" + str(plimbs) + "\n"
        # S = S + "fp_mult_"+ str(plimbs) + "x" + str(plimbs) + ":\n"
        # S = S + "    mult_"+ str(plimbs) + "x" + str(plimbs) + "\n"
        # S = S + "    ret\n"

        print(S)

        print("\n")

if __name__ == "__main__":
    main(sys.argv[1:])
