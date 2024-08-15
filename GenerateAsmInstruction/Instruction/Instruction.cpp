#include <iostream>
#include <windows.h>

/*
* ⚠️⚠️⚠️⚠️⚠️⚠️⚠️⚠️⚠️⚠️⚠️⚠️⚠️⚠️⚠️⚠️⚠️⚠️⚠️⚠️⚠️
* 1.Release x64
* 2.常规
* 平台工具集(LLVM (clang-cl))
* 3.C/C++
* 优化: 优化(已禁用)
* 代码生成: 运行库(多线程)、安全检查(禁用安全检查)
* 4.链接器
* 清单文件: 生成清单(否)
* 调试: 生成调试信息(否)
*/

using namespace std;

#pragma code_seg(".call")

void AsmCall(DWORD_PTR opAddr1, PDWORD_PTR pVtRegs) {
    // 保存真实栈顶栈底
    DWORD_PTR realRSP;
    DWORD_PTR realRBP;
    __asm {
        mov realRSP, rsp
        mov realRBP, rbp
    }

    // Window API 地址
    DWORD_PTR winApiAddr = *(PDWORD_PTR)opAddr1;

    // 虚拟寄存器 覆盖 真实寄存器
    DWORD_PTR vtRAX = pVtRegs[0];
    DWORD_PTR vtRBX = pVtRegs[1];
    DWORD_PTR vtRCX = pVtRegs[2];
    DWORD_PTR vtRDX = pVtRegs[3];
    DWORD_PTR vtRSI = pVtRegs[4];
    DWORD_PTR vtRDI = pVtRegs[5];
    DWORD_PTR vtR8 = pVtRegs[6];
    DWORD_PTR vtR9 = pVtRegs[7];
    DWORD_PTR vtR10 = pVtRegs[8];
    DWORD_PTR vtR11 = pVtRegs[9];
    DWORD_PTR vtR12 = pVtRegs[10];
    DWORD_PTR vtR13 = pVtRegs[11];
    DWORD_PTR vtR14 = pVtRegs[12];
    DWORD_PTR vtR15 = pVtRegs[13];
    DWORD_PTR vtRSP = pVtRegs[14];
    DWORD_PTR vtRBP = pVtRegs[15];
    __asm {
        mov rax, vtRAX
        mov rbx, vtRBX
        mov rcx, vtRCX
        mov rdx, vtRDX
        mov rsi, vtRSI
        mov rdi, vtRDI
        mov r8, vtR8
        mov r9, vtR9
        mov r10, vtR10
        mov r11, vtR11
        mov r12, vtR12
        mov r13, vtR13
        mov r14, vtR14
        mov r15, vtR15
        mov rsp, vtRSP
        // mov rbp, vtRBP (与 Call 冲突)
    }

    // 调用 Windows API
    __asm {
        call qword ptr[winApiAddr] // (call qword ptr [rbp])
    }

    // 保存调用 Windows API 后真实寄存器的值
    __asm {
        push rax
        push rbx
        push rcx
        push rdx
        push rsi
        push rdi
        push r8
        push r9
        push r10
        push r11
        push r12
        push r13
        push r14
        push r15
        push rsp
        push rbp
    }

    // 真实寄存器 覆盖 虚拟寄存器
    DWORD_PTR currentRSP;
    __asm {
        mov currentRSP, rsp;
    }
    pVtRegs[0] = *(PDWORD_PTR)(currentRSP + 0x78); // RAX
    pVtRegs[1] = *(PDWORD_PTR)(currentRSP + 0x70); // RBX
    pVtRegs[2] = *(PDWORD_PTR)(currentRSP + 0x68); // RCX
    pVtRegs[3] = *(PDWORD_PTR)(currentRSP + 0x60); // RDX
    pVtRegs[4] = *(PDWORD_PTR)(currentRSP + 0x58); // RSI
    pVtRegs[5] = *(PDWORD_PTR)(currentRSP + 0x50); // RDI
    pVtRegs[6] = *(PDWORD_PTR)(currentRSP + 0x48); // R8
    pVtRegs[7] = *(PDWORD_PTR)(currentRSP + 0x40); // R9
    pVtRegs[8] = *(PDWORD_PTR)(currentRSP + 0x38); // R10
    pVtRegs[9] = *(PDWORD_PTR)(currentRSP + 0x30); // R11
    pVtRegs[10] = *(PDWORD_PTR)(currentRSP + 0x28); // R12
    pVtRegs[11] = *(PDWORD_PTR)(currentRSP + 0x20); // R13
    pVtRegs[12] = *(PDWORD_PTR)(currentRSP + 0x18); // R14
    pVtRegs[13] = *(PDWORD_PTR)(currentRSP + 0x10); // R15
    pVtRegs[14] = *(PDWORD_PTR)(currentRSP + 0x08) + 0x70; // RSP
    pVtRegs[15] = *(PDWORD_PTR)(currentRSP + 0x00); // RBP

    // 还原真实栈顶栈底
    __asm {
        mov rsp, realRSP
        mov rbp, realRBP
    }
}

#pragma code_seg(".movsx")

void AsmMovsx(char opBit1, DWORD_PTR opAddr1, char opBit2, DWORD_PTR opAddr2) {
    __asm {
        mov r8, qword ptr[opAddr1]
        mov r9, qword ptr[opAddr2]
        mov r10, qword ptr[r8]
        mov r11, qword ptr[r9]
    }
    if (opBit1 == 'q' || opBit1 == 'd') {
        if (opBit2 == 'w') {
            __asm {
                movsx r10, r11w
            }
        }
        else if (opBit2 == 'b') {
            __asm {
                movsx r10, r11b
            }
        }
    }
    else if (opBit1 == 'w') {
        __asm {
            movsx r10w, r11b
        }
    }
    __asm {
        mov qword ptr[r8], r10
    }
}

#pragma code_seg(".movzx")

void AsmMovzx(char opBit1, DWORD_PTR opAddr1, char opBit2, DWORD_PTR opAddr2) {
    __asm {
        mov r8, qword ptr[opAddr1]
        mov r9, qword ptr[opAddr2]
        mov r10, qword ptr[r8]
        mov r11, qword ptr[r9]
    }
    if (opBit1 == 'q' || opBit1 == 'd') {
        if (opBit2 == 'w') {
            __asm {
                movzx r10, r11w
            }
        }
        else if (opBit2 == 'b') {
            __asm {
                movzx r10, r11b
            }
        }
    }
    else if (opBit1 == 'w') {
        __asm {
            movzx r10w, r11b
        }
    }
    __asm {
        mov qword ptr[r8], r10
    }
}

#pragma code_seg(".cmp")

void AsmCmp(char opBit1, DWORD_PTR opAddr1, DWORD_PTR opAddr2, PDWORD_PTR pVtRegs) {
    __asm {
        mov r8, qword ptr[opAddr1]
        mov r9, qword ptr[opAddr2]
        mov r10, qword ptr[r8]
        mov r11, qword ptr[r9]
    }
    DWORD_PTR vtEFL;
    if (opBit1 == 'q') {
        __asm {
            cmp r10, r11
            pushf
            pop rax
            mov vtEFL, rax
        }
    }
    if (opBit1 == 'd') {
        __asm {
            cmp r10d, r11d
            pushf
            pop rax
            mov vtEFL, rax
        }
    }
    if (opBit1 == 'w') {
        __asm {
            cmp r10w, r11w
            pushf
            pop rax
            mov vtEFL, rax
        }
    }
    if (opBit1 == 'b') {
        __asm {
            cmp r10b, r11b
            pushf
            pop rax
            mov vtEFL, rax
        }
    }
    pVtRegs[17] = vtEFL;
}

#pragma code_seg(".test")

void AsmTest(char opBit1, DWORD_PTR opAddr1, DWORD_PTR opAddr2, PDWORD_PTR pVtRegs) {
    __asm {
        mov r8, qword ptr[opAddr1]
        mov r9, qword ptr[opAddr2]
        mov r10, qword ptr[r8]
        mov r11, qword ptr[r9]
    }
    DWORD_PTR vtEFL;
    if (opBit1 == 'q') {
        __asm {
            test r10, r11
            pushf
            pop rax
            mov vtEFL, rax
        }
    }
    if (opBit1 == 'd') {
        __asm {
            test r10d, r11d
            pushf
            pop rax
            mov vtEFL, rax
        }
    }
    if (opBit1 == 'w') {
        __asm {
            test r10w, r11w
            pushf
            pop rax
            mov vtEFL, rax
        }
    }
    if (opBit1 == 'b') {
        __asm {
            test r10b, r11b
            pushf
            pop rax
            mov vtEFL, rax
        }
    }
    pVtRegs[17] = vtEFL;
}

#pragma code_seg(".je")

int AsmJe(DWORD_PTR vtEFL) {
    int isJmp = 1;
    __asm {
        mov rax, vtEFL
        push rax
        popf
        je jmp
        mov isJmp, 0x00
        jmp :
    }
    return isJmp;
}

#pragma code_seg(".text")

int main() {
}