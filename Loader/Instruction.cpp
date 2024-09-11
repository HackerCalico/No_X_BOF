#include <iostream>
#include <windows.h>

using namespace std;

void Push(char opType1, char opBit1, DWORD_PTR opAddr1, char opBit2, DWORD_PTR opAddr2, PDWORD_PTR vtRegs, char* obfMap) {
    vtRegs[14] -= sizeof(DWORD_PTR);
    *(PDWORD_PTR)(vtRegs[14]) = *(PDWORD_PTR)(opAddr1);
}

void Pop(char opType1, char opBit1, DWORD_PTR opAddr1, char opBit2, DWORD_PTR opAddr2, PDWORD_PTR vtRegs, char* obfMap) {
    *(PDWORD_PTR)(opAddr1) = *(PDWORD_PTR)(vtRegs[14]);
    vtRegs[14] += sizeof(DWORD_PTR);
}

void Call(char opType1, char opBit1, DWORD_PTR opAddr1, char opBit2, DWORD_PTR opAddr2, PDWORD_PTR vtRegs, char* obfMap) {
    // ±£´æÕæÊµÕ»¶¥Õ»µ×
    DWORD_PTR realRSP;
    DWORD_PTR realRBP;
    __asm {
        mov realRSP, rsp
        mov realRBP, rbp
    }

    DWORD_PTR winApiAddr = *(PDWORD_PTR)opAddr1;

    // ÐéÄâ¼Ä´æÆ÷ ¸²¸Ç ÕæÊµ¼Ä´æÆ÷
    DWORD_PTR vtRAX = vtRegs[0];
    DWORD_PTR vtRBX = vtRegs[1];
    DWORD_PTR vtRCX = vtRegs[2];
    DWORD_PTR vtRDX = vtRegs[3];
    DWORD_PTR vtRSI = vtRegs[4];
    DWORD_PTR vtRDI = vtRegs[5];
    DWORD_PTR vtR8 = vtRegs[6];
    DWORD_PTR vtR9 = vtRegs[7];
    DWORD_PTR vtR10 = vtRegs[8];
    DWORD_PTR vtR11 = vtRegs[9];
    DWORD_PTR vtR12 = vtRegs[10];
    DWORD_PTR vtR13 = vtRegs[11];
    DWORD_PTR vtR14 = vtRegs[12];
    DWORD_PTR vtR15 = vtRegs[13];
    DWORD_PTR vtRSP = vtRegs[14];
    // DWORD_PTR vtRBP = vtRegs[15];
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
        // mov rbp, vtRBP (Óë call ³åÍ»)
    }

    // µ÷ÓÃ Windows API
    __asm {
        call winApiAddr // (call qword ptr [rbp + 0x48])
    }

    // ÕæÊµ¼Ä´æÆ÷ ¸²¸Ç ÐéÄâ¼Ä´æÆ÷
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
    DWORD_PTR currentRSP;
    __asm {
        mov currentRSP, rsp;
    }
    vtRegs[0] = *(PDWORD_PTR)(currentRSP + 0x78);
    vtRegs[1] = *(PDWORD_PTR)(currentRSP + 0x70);
    vtRegs[2] = *(PDWORD_PTR)(currentRSP + 0x68);
    vtRegs[3] = *(PDWORD_PTR)(currentRSP + 0x60);
    vtRegs[4] = *(PDWORD_PTR)(currentRSP + 0x58);
    vtRegs[5] = *(PDWORD_PTR)(currentRSP + 0x50);
    vtRegs[6] = *(PDWORD_PTR)(currentRSP + 0x48);
    vtRegs[7] = *(PDWORD_PTR)(currentRSP + 0x40);
    vtRegs[8] = *(PDWORD_PTR)(currentRSP + 0x38);
    vtRegs[9] = *(PDWORD_PTR)(currentRSP + 0x30);
    vtRegs[10] = *(PDWORD_PTR)(currentRSP + 0x28);
    vtRegs[11] = *(PDWORD_PTR)(currentRSP + 0x20);
    vtRegs[12] = *(PDWORD_PTR)(currentRSP + 0x18);
    vtRegs[13] = *(PDWORD_PTR)(currentRSP + 0x10);
    vtRegs[14] = *(PDWORD_PTR)(currentRSP + 0x08) + 0x70; // RSP
    // vtRegs[15] = *(PDWORD_PTR)(currentRSP + 0x00); // RBP
    
    // »¹Ô­ÕæÊµÕ»¶¥Õ»µ×
    __asm {
        mov rsp, realRSP
        mov rbp, realRBP
    }
}

void Ret(char opType1, char opBit1, DWORD_PTR opAddr1, char opBit2, DWORD_PTR opAddr2, PDWORD_PTR vtRegs, char* obfMap) {
    vtRegs[16] = *(PDWORD_PTR)vtRegs[14];
    vtRegs[14] = vtRegs[14] + sizeof(DWORD_PTR);
}

void Mov(char opType1, char opBit1, DWORD_PTR opAddr1, char opBit2, DWORD_PTR opAddr2, PDWORD_PTR vtRegs, char* obfMap) {
    if (opBit1 == obfMap['q']) {
        *(PDWORD64)opAddr1 = *(PDWORD64)opAddr2;
    }
    else if (opBit1 == obfMap['d']) {
        if (opType1 == 'r') {
            *(PDWORD_PTR)opAddr1 = *(PDWORD)opAddr2;
        }
        else {
            *(PDWORD)opAddr1 = *(PDWORD)opAddr2;
        }
    }
    else if (opBit1 == obfMap['w']) {
        *(PWORD)opAddr1 = *(PWORD)opAddr2;
    }
    else if (opBit1 == obfMap['b']) {
        *(PBYTE)opAddr1 = *(PBYTE)opAddr2;
    }
}

void Movzx(char opType1, char opBit1, DWORD_PTR opAddr1, char opBit2, DWORD_PTR opAddr2, PDWORD_PTR vtRegs, char* obfMap) {
    __asm {
        mov r12, qword ptr[opAddr1]
        mov r13, qword ptr[opAddr2]
        mov r14, qword ptr[r12]
        mov r15, qword ptr[r13]
    }
    if (opBit1 == obfMap['q'] || opBit1 == obfMap['d']) {
        if (opBit2 == obfMap['w']) {
            __asm {
                movzx r14, r15w
            }
        }
        else if (opBit2 == obfMap['b']) {
            __asm {
                movzx r14, r15b
            }
        }
    }
    else if (opBit1 == obfMap['w']) {
        __asm {
            movzx r14w, r15b
        }
    }
    __asm {
        mov qword ptr[r12], r14
    }
}

void Movabs(char opType1, char opBit1, DWORD_PTR opAddr1, char opBit2, DWORD_PTR opAddr2, PDWORD_PTR vtRegs, char* obfMap) {
    Mov(opType1, opBit1, opAddr1, opBit2, opAddr2, vtRegs, obfMap);
}

void Movsxd(char opType1, char opBit1, DWORD_PTR opAddr1, char opBit2, DWORD_PTR opAddr2, PDWORD_PTR vtRegs, char* obfMap) {
    *(PDWORD64)opAddr1 = *(PDWORD)opAddr2;
}

void Lea(char opType1, char opBit1, DWORD_PTR opAddr1, char opBit2, DWORD_PTR opAddr2, PDWORD_PTR vtRegs, char* obfMap) {
    Mov(opType1, opBit1, opAddr1, opBit2, opAddr2, vtRegs, obfMap);
}

void Nop(char opType1, char opBit1, DWORD_PTR opAddr1, char opBit2, DWORD_PTR opAddr2, PDWORD_PTR vtRegs, char* obfMap) {}

void Add(char opType1, char opBit1, DWORD_PTR opAddr1, char opBit2, DWORD_PTR opAddr2, PDWORD_PTR vtRegs, char* obfMap) {
    if (opBit1 == obfMap['q']) {
        *(PDWORD64)opAddr1 += *(PDWORD64)opAddr2;
    }
    else if (opBit1 == obfMap['d']) {
        if (opType1 == 'r') {
            *(PDWORD_PTR)opAddr1 = *(PDWORD)opAddr1 + *(PDWORD)opAddr2;
        }
        else {
            *(PDWORD)opAddr1 += *(PDWORD)opAddr2;
        }
    }
    else if (opBit1 == obfMap['w']) {
        *(PWORD)opAddr1 += *(PWORD)opAddr2;
    }
    else if (opBit1 == obfMap['b']) {
        *(PBYTE)opAddr1 += *(PBYTE)opAddr2;
    }
}

void Inc(char opType1, char opBit1, DWORD_PTR opAddr1, char opBit2, DWORD_PTR opAddr2, PDWORD_PTR vtRegs, char* obfMap) {
    if (opBit1 == obfMap['q']) {
        *(PDWORD64)opAddr1 += 1;
    }
    else if (opBit1 == obfMap['d']) {
        if (opType1 == 'r') {
            *(PDWORD_PTR)opAddr1 = *(PDWORD)opAddr1 + 1;
        }
        else {
            *(PDWORD)opAddr1 += 1;
        }
    }
    else if (opBit1 == obfMap['w']) {
        *(PWORD)opAddr1 += 1;
    }
    else if (opBit1 == obfMap['b']) {
        *(PBYTE)opAddr1 += 1;
    }
}

void Sub(char opType1, char opBit1, DWORD_PTR opAddr1, char opBit2, DWORD_PTR opAddr2, PDWORD_PTR vtRegs, char* obfMap) {
    if (opBit1 == obfMap['q']) {
        *(PDWORD64)opAddr1 -= *(PDWORD64)opAddr2;
    }
    else if (opBit1 == obfMap['d']) {
        if (opType1 == 'r') {
            *(PDWORD_PTR)opAddr1 = *(PDWORD)opAddr1 - *(PDWORD)opAddr2;
        }
        else {
            *(PDWORD)opAddr1 -= *(PDWORD)opAddr2;
        }
    }
    else if (opBit1 == obfMap['w']) {
        *(PWORD)opAddr1 -= *(PWORD)opAddr2;
    }
    else if (opBit1 == obfMap['b']) {
        *(PBYTE)opAddr1 -= *(PBYTE)opAddr2;
    }
}

void Cmp(char opType1, char opBit1, DWORD_PTR opAddr1, char opBit2, DWORD_PTR opAddr2, PDWORD_PTR vtRegs, char* obfMap) {
    __asm {
        mov r12, qword ptr[opAddr1]
        mov r13, qword ptr[opAddr2]
        mov r14, qword ptr[r12]
        mov r15, qword ptr[r13]
    }
    DWORD_PTR vtEFL;
    if (opBit1 == obfMap['q']) {
        __asm {
            cmp r14, r15
            pushf
            pop rax
            mov vtEFL, rax
        }
    }
    if (opBit1 == obfMap['d']) {
        __asm {
            cmp r14d, r15d
            pushf
            pop rax
            mov vtEFL, rax
        }
    }
    if (opBit1 == obfMap['w']) {
        __asm {
            cmp r14w, r15w
            pushf
            pop rax
            mov vtEFL, rax
        }
    }
    if (opBit1 == obfMap['b']) {
        __asm {
            cmp r14b, r15b
            pushf
            pop rax
            mov vtEFL, rax
        }
    }
    vtRegs[17] = vtEFL;
}

void Imul(char opType1, char opBit1, DWORD_PTR opAddr1, char opBit2, DWORD_PTR opAddr2, PDWORD_PTR vtRegs, char* obfMap) {
    if (opBit1 == obfMap['q']) {
        *(PDWORD64)opAddr1 *= *(PDWORD64)opAddr2;
    }
    else if (opBit1 == obfMap['d']) {
        if (opType1 == 'r') {
            *(PDWORD_PTR)opAddr1 = *(PDWORD)opAddr1 * *(PDWORD)opAddr2;
        }
        else {
            *(PDWORD)opAddr1 *= *(PDWORD)opAddr2;
        }
    }
    else if (opBit1 == obfMap['w']) {
        *(PWORD)opAddr1 *= *(PWORD)opAddr2;
    }
    else if (opBit1 == obfMap['b']) {
        *(PBYTE)opAddr1 *= *(PBYTE)opAddr2;
    }
}

void And(char opType1, char opBit1, DWORD_PTR opAddr1, char opBit2, DWORD_PTR opAddr2, PDWORD_PTR vtRegs, char* obfMap) {
    if (opBit1 == obfMap['q']) {
        *(PDWORD64)opAddr1 &= *(PDWORD64)opAddr2;
    }
    else if (opBit1 == obfMap['d']) {
        if (opType1 == 'r') {
            *(PDWORD_PTR)opAddr1 = *(PDWORD)opAddr1 & *(PDWORD)opAddr2;
        }
        else {
            *(PDWORD)opAddr1 &= *(PDWORD)opAddr2;
        }
    }
    else if (opBit1 == obfMap['w']) {
        *(PWORD)opAddr1 &= *(PWORD)opAddr2;
    }
    else if (opBit1 == obfMap['b']) {
        *(PBYTE)opAddr1 &= *(PBYTE)opAddr2;
    }
}

void Or(char opType1, char opBit1, DWORD_PTR opAddr1, char opBit2, DWORD_PTR opAddr2, PDWORD_PTR vtRegs, char* obfMap) {
    if (opBit1 == obfMap['q']) {
        *(PDWORD64)opAddr1 |= *(PDWORD64)opAddr2;
    }
    else if (opBit1 == obfMap['d']) {
        if (opType1 == 'r') {
            *(PDWORD_PTR)opAddr1 = *(PDWORD)opAddr1 | *(PDWORD)opAddr2;
        }
        else {
            *(PDWORD)opAddr1 |= *(PDWORD)opAddr2;
        }
    }
    else if (opBit1 == obfMap['w']) {
        *(PWORD)opAddr1 |= *(PWORD)opAddr2;
    }
    else if (opBit1 == obfMap['b']) {
        *(PBYTE)opAddr1 |= *(PBYTE)opAddr2;
    }
}

void Xor(char opType1, char opBit1, DWORD_PTR opAddr1, char opBit2, DWORD_PTR opAddr2, PDWORD_PTR vtRegs, char* obfMap) {
    if (opBit1 == obfMap['q']) {
        *(PDWORD64)opAddr1 ^= *(PDWORD64)opAddr2;
    }
    else if (opBit1 == obfMap['d']) {
        if (opType1 == 'r') {
            *(PDWORD_PTR)opAddr1 = *(PDWORD)opAddr1 ^ *(PDWORD)opAddr2;
        }
        else {
            *(PDWORD)opAddr1 ^= *(PDWORD)opAddr2;
        }
    }
    else if (opBit1 == obfMap['w']) {
        *(PWORD)opAddr1 ^= *(PWORD)opAddr2;
    }
    else if (opBit1 == obfMap['b']) {
        *(PBYTE)opAddr1 ^= *(PBYTE)opAddr2;
    }
}

void Test(char opType1, char opBit1, DWORD_PTR opAddr1, char opBit2, DWORD_PTR opAddr2, PDWORD_PTR vtRegs, char* obfMap) {
    __asm {
        mov r12, qword ptr[opAddr1]
        mov r13, qword ptr[opAddr2]
        mov r14, qword ptr[r12]
        mov r15, qword ptr[r13]
    }
    DWORD_PTR vtEFL;
    if (opBit1 == obfMap['q']) {
        __asm {
            test r14, r15
            pushf
            pop rax
            mov vtEFL, rax
        }
    }
    if (opBit1 == obfMap['d']) {
        __asm {
            test r14d, r15d
            pushf
            pop rax
            mov vtEFL, rax
        }
    }
    if (opBit1 == obfMap['w']) {
        __asm {
            test r14w, r15w
            pushf
            pop rax
            mov vtEFL, rax
        }
    }
    if (opBit1 == obfMap['b']) {
        __asm {
            test r14b, r15b
            pushf
            pop rax
            mov vtEFL, rax
        }
    }
    vtRegs[17] = vtEFL;
}

void Shl(char opType1, char opBit1, DWORD_PTR opAddr1, char opBit2, DWORD_PTR opAddr2, PDWORD_PTR vtRegs, char* obfMap) {
    if (opBit1 == obfMap['q']) {
        *(PDWORD64)opAddr1 <<= *(PDWORD64)opAddr2;
    }
    else if (opBit1 == obfMap['d']) {
        if (opType1 == 'r') {
            *(PDWORD_PTR)opAddr1 = *(PDWORD)opAddr1 << *(PDWORD)opAddr2;
        }
        else {
            *(PDWORD)opAddr1 <<= *(PDWORD)opAddr2;
        }
    }
    else if (opBit1 == obfMap['w']) {
        *(PWORD)opAddr1 <<= *(PWORD)opAddr2;
    }
    else if (opBit1 == obfMap['b']) {
        *(PBYTE)opAddr1 <<= *(PBYTE)opAddr2;
    }
}

void Shr(char opType1, char opBit1, DWORD_PTR opAddr1, char opBit2, DWORD_PTR opAddr2, PDWORD_PTR vtRegs, char* obfMap) {
    if (opBit1 == obfMap['q']) {
        *(PDWORD64)opAddr1 >>= *(PDWORD64)opAddr2;
    }
    else if (opBit1 == obfMap['d']) {
        if (opType1 == 'r') {
            *(PDWORD_PTR)opAddr1 = *(PDWORD)opAddr1 >> *(PDWORD)opAddr2;
        }
        else {
            *(PDWORD)opAddr1 >>= *(PDWORD)opAddr2;
        }
    }
    else if (opBit1 == obfMap['w']) {
        *(PWORD)opAddr1 >>= *(PWORD)opAddr2;
    }
    else if (opBit1 == obfMap['b']) {
        *(PBYTE)opAddr1 >>= *(PBYTE)opAddr2;
    }
}

void Cdqe(char opType1, char opBit1, DWORD_PTR opAddr1, char opBit2, DWORD_PTR opAddr2, PDWORD_PTR vtRegs, char* obfMap) {
    vtRegs[0] = (DWORD)vtRegs[0];
}

void RepStosb(char opType1, char opBit1, DWORD_PTR opAddr1, char opBit2, DWORD_PTR opAddr2, PDWORD_PTR vtRegs, char* obfMap) {
    while (vtRegs[2]) { // vtRCX
        *(PBYTE)vtRegs[5] = *(PBYTE)&vtRegs[0]; // byte ptr [vtRDI] = al
        vtRegs[5]++;
        vtRegs[2]--;
    }
}

void RepMovsb(char opType1, char opBit1, DWORD_PTR opAddr1, char opBit2, DWORD_PTR opAddr2, PDWORD_PTR vtRegs, char* obfMap) {
    while (vtRegs[2]) { // vtRCX
        *(PBYTE)vtRegs[5] = *(PBYTE)vtRegs[4]; // byte ptr [vtRDI] = byte ptr [vtRSI]
        vtRegs[4]++;
        vtRegs[5]++;
        vtRegs[2]--;
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

int Jbe(DWORD_PTR vtEFL) {
    int isJmp = 1;
    __asm {
        mov rax, vtEFL
        push rax
        popf
        jbe jmp
        mov isJmp, 0x00
        jmp :
    }
    return isJmp;
}

int Jl(DWORD_PTR vtEFL) {
    int isJmp = 1;
    __asm {
        mov rax, vtEFL
        push rax
        popf
        jl jmp
        mov isJmp, 0x00
        jmp :
    }
    return isJmp;
}

int Jge(DWORD_PTR vtEFL) {
    int isJmp = 1;
    __asm {
        mov rax, vtEFL
        push rax
        popf
        jge jmp
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

void Jcc(PVOID pInstructionFunc, DWORD_PTR opAddr1, PDWORD_PTR vtRegs) {
    DWORD_PTR vtEFL = vtRegs[17];
    int isJmp = ((int(*)(...))pInstructionFunc)(vtEFL);
    if (isJmp) {
        vtRegs[16] = *(PDWORD_PTR)opAddr1;
    }
}

// µ÷ÓÃÖ¸Áî
int InvokeInstruction(int mnemonicIndex, char opType1, char opBit1, DWORD_PTR opAddr1, char opBit2, DWORD_PTR opAddr2, PDWORD_PTR vtRegs, char* obfMap) {
    int jmpIndex = 24;
    PVOID mnemonicMap[] = { Push, Pop, Call, Ret, Mov, Movzx, Movabs, Movsxd, Lea, Nop, Add, Inc, Sub, Cmp, Imul, And, Or, Xor, Test, Shl, Shr, Cdqe, RepStosb, RepMovsb, Jmp, Je, Jne, Jbe, Jl, Jge, Jle };
    PVOID pInstructionFunc = mnemonicMap[mnemonicIndex];
    DWORD_PTR vtRIP = vtRegs[16];

    if (mnemonicIndex < jmpIndex) {
        ((void(*)(...))pInstructionFunc)(opType1, opBit1, opAddr1, opBit2, opAddr2, vtRegs, obfMap);
    }
    else {
        Jcc(pInstructionFunc, opAddr1, vtRegs);
    }

    // ·¢ÉúÌø×ª
    if (vtRegs[16] != vtRIP) {
        return 1;
    }
    return 0;
}