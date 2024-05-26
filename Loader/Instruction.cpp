#include <iostream>
#include <windows.h>

using namespace std;

void Push(DWORD_PTR opAddr1, PDWORD_PTR pVtRegs) {
    pVtRegs[14] -= sizeof(DWORD_PTR);
    *(PDWORD_PTR)(pVtRegs[14]) = *(PDWORD_PTR)(opAddr1);
}

void Pop(DWORD_PTR opAddr1, PDWORD_PTR pVtRegs) {
    *(PDWORD_PTR)(opAddr1) = *(PDWORD_PTR)(pVtRegs[14]);
    pVtRegs[14] += sizeof(DWORD_PTR);
}

void Call(DWORD_PTR opAddr1, PDWORD_PTR pVtRegs) {
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

void Ret(DWORD_PTR opAddr1, PDWORD_PTR pVtRegs) { }

void Mov(char opType1, char opBit1, DWORD_PTR opAddr1, char opBit2, DWORD_PTR opAddr2) {
    switch (opBit1)
    {
    case 'q':
        *(PDWORD64)opAddr1 = *(PDWORD64)opAddr2;
        break;
    case 'd':
        if (opType1 == 'r') {
            *(PDWORD_PTR)opAddr1 = *(PDWORD)opAddr2;
        }
        else {
            *(PDWORD)opAddr1 = *(PDWORD)opAddr2;
        }
        break;
    case 'w':
        *(PWORD)opAddr1 = *(PWORD)opAddr2;
        break;
    case 'b':
        *(PBYTE)opAddr1 = *(PBYTE)opAddr2;
        break;
    }
}

void Movsxd(char opType1, char opBit1, DWORD_PTR opAddr1, char opBit2, DWORD_PTR opAddr2) {
    *(PDWORD64)opAddr1 = *(PDWORD)opAddr2;
}

void Lea(char opType1, char opBit1, DWORD_PTR opAddr1, char opBit2, DWORD_PTR opAddr2) {
    Mov(opType1, opBit1, opAddr1, opBit2, opAddr2);
}

void Add(char opType1, char opBit1, DWORD_PTR opAddr1, char opBit2, DWORD_PTR opAddr2) {
    switch (opBit1)
    {
    case 'q':
        *(PDWORD64)opAddr1 += *(PDWORD64)opAddr2;
        break;
    case 'd':
        if (opType1 == 'r') {
            *(PDWORD_PTR)opAddr1 = *(PDWORD)opAddr1 + *(PDWORD)opAddr2;
        }
        else {
            *(PDWORD)opAddr1 += *(PDWORD)opAddr2;
        }
        break;
    case 'w':
        *(PWORD)opAddr1 += *(PWORD)opAddr2;
        break;
    case 'b':
        *(PBYTE)opAddr1 += *(PBYTE)opAddr2;
        break;
    }
}

void Inc(char opType1, char opBit1, DWORD_PTR opAddr1) {
    switch (opBit1)
    {
    case 'q':
        *(PDWORD64)opAddr1 += 1;
        break;
    case 'd':
        if (opType1 == 'r') {
            *(PDWORD_PTR)opAddr1 = *(PDWORD)opAddr1 + 1;
        }
        else {
            *(PDWORD)opAddr1 += 1;
        }
        break;
    case 'w':
        *(PWORD)opAddr1 += 1;
        break;
    case 'b':
        *(PBYTE)opAddr1 += 1;
        break;
    }
}

void Sub(char opType1, char opBit1, DWORD_PTR opAddr1, char opBit2, DWORD_PTR opAddr2) {
    switch (opBit1)
    {
    case 'q':
        *(PDWORD64)opAddr1 -= *(PDWORD64)opAddr2;
        break;
    case 'd':
        if (opType1 == 'r') {
            *(PDWORD_PTR)opAddr1 = *(PDWORD)opAddr1 - *(PDWORD)opAddr2;
        }
        else {
            *(PDWORD)opAddr1 -= *(PDWORD)opAddr2;
        }
        break;
    case 'w':
        *(PWORD)opAddr1 -= *(PWORD)opAddr2;
        break;
    case 'b':
        *(PBYTE)opAddr1 -= *(PBYTE)opAddr2;
        break;
    }
}

void Cmp(char opBit1, PDWORD_PTR pVtRegs) {
    DWORD_PTR vtEFL;
    switch (opBit1)
    {
    case 'q':
        __asm {
            cmp r10, r11
            pushf
            pop rax
            mov vtEFL, rax
        }
        break;
    case 'd':
        __asm {
            cmp r10d, r11d
            pushf
            pop rax
            mov vtEFL, rax
        }
        break;
    case 'w':
        __asm {
            cmp r10w, r11w
            pushf
            pop rax
            mov vtEFL, rax
        }
        break;
    case 'b':
        __asm {
            cmp r10b, r11b
            pushf
            pop rax
            mov vtEFL, rax
        }
        break;
    }
    pVtRegs[17] = vtEFL;
}

void Xor(char opType1, char opBit1, DWORD_PTR opAddr1, char opBit2, DWORD_PTR opAddr2) {
    switch (opBit1)
    {
    case 'q':
        *(PDWORD64)opAddr1 ^= *(PDWORD64)opAddr2;
        break;
    case 'd':
        if (opType1 == 'r') {
            *(PDWORD_PTR)opAddr1 = *(PDWORD)opAddr1 ^ *(PDWORD)opAddr2;
        }
        else {
            *(PDWORD)opAddr1 ^= *(PDWORD)opAddr2;
        }
        break;
    case 'w':
        *(PWORD)opAddr1 ^= *(PWORD)opAddr2;
        break;
    case 'b':
        *(PBYTE)opAddr1 ^= *(PBYTE)opAddr2;
        break;
    }
}

void Test(char opBit1, PDWORD_PTR pVtRegs) {
    DWORD_PTR vtEFL;
    switch (opBit1)
    {
    case 'q':
        __asm {
            test r10, r11
            pushf
            pop rax
            mov vtEFL, rax
        }
        break;
    case 'd':
        __asm {
            test r10d, r11d
            pushf
            pop rax
            mov vtEFL, rax
        }
        break;
    case 'w':
        __asm {
            test r10w, r11w
            pushf
            pop rax
            mov vtEFL, rax
        }
        break;
    case 'b':
        __asm {
            test r10b, r11b
            pushf
            pop rax
            mov vtEFL, rax
        }
        break;
    }
    pVtRegs[17] = vtEFL;
}

void Cdqe(PDWORD_PTR pVtRegs) {
    pVtRegs[0] = (DWORD)pVtRegs[0];
}

void RepStosb(PDWORD_PTR pVtRegs) {
    while (pVtRegs[2]) { // vtRCX
        *(PBYTE)pVtRegs[5] = *(PBYTE)&pVtRegs[0]; // byte ptr [vtRDI] = al
        pVtRegs[5]++;
        pVtRegs[2]--;
    }
}

int Jmp(DWORD_PTR vtEFL) {
    return 1;
}

int Je(DWORD_PTR vtEFL) {
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

int Jne(DWORD_PTR vtEFL) {
    int isJmp = 1;
    __asm {
        mov rax, vtEFL
        push rax
        popf
        jne jmp
        mov isJmp, 0x00
        jmp :
    }
    return isJmp;
}

int Jle(DWORD_PTR vtEFL) {
    int isJmp = 1;
    __asm {
        mov rax, vtEFL
        push rax
        popf
        jle jmp
        mov isJmp, 0x00
        jmp :
    }
    return isJmp;
}

void Jcc(PVOID instructionFunc, DWORD_PTR opAddr1, PDWORD_PTR pVtRegs) {
    DWORD_PTR vtEFL = pVtRegs[17];
    int isJmp = ((int(*)(...))instructionFunc)(vtEFL);
    if (isJmp) {
        DWORD_PTR vtRIP = *(PDWORD_PTR)opAddr1;
        pVtRegs[16] = vtRIP;
    }
}

// 调用指令
int InvokeInstruction(int mnemonicIndex, char opType1, char opBit1, DWORD_PTR opAddr1, char opBit2, DWORD_PTR opAddr2, PDWORD_PTR pVtRegs) {
    PVOID mnemonicMapping[] = { Push, Pop, Call, Ret, Mov, Movsxd, Lea, Add, Inc, Sub, Cmp, Xor, Test, Cdqe, RepStosb, Jmp, Je, Jne, Jle };
    PVOID instructionFunc = mnemonicMapping[mnemonicIndex];
    DWORD_PTR vtRIP = pVtRegs[16];

    // 调用指令函数
    // Push - Ret
    if (mnemonicIndex < 4) {
        ((void(*)(...))instructionFunc)(opAddr1, pVtRegs);
    }
    else if (instructionFunc == Cmp || instructionFunc == Test) {
        __asm {
            mov r8, qword ptr[opAddr1]
            mov r9, qword ptr[opAddr2]
            mov r10, qword ptr[r8]
            mov r11, qword ptr[r9]
        }
        ((void(*)(...))instructionFunc)(opBit1, pVtRegs);
    }
    else if (instructionFunc == Cdqe || instructionFunc == RepStosb) {
        ((void(*)(...))instructionFunc)(pVtRegs);
    }
    // Jcc
    else if (mnemonicIndex > 14) {
        Jcc(instructionFunc, opAddr1, pVtRegs);
    }
    // 其他一个操作数的指令
    else if (opAddr2 == NULL) {
        ((void(*)(...))instructionFunc)(opType1, opBit1, opAddr1);
    }
    // 其他两个操作数的指令
    else {
        ((void(*)(...))instructionFunc)(opType1, opBit1, opAddr1, opBit2, opAddr2);
    }

    // 发生跳转
    if (pVtRegs[16] != vtRIP) {
        return 1;
    }
    return 0;
}