#include <iostream>
#include <windows.h>

#include "AsmInstruction.h"

using namespace std;

void Push(DWORD_PTR opAddr1, PDWORD_PTR pVtRegs) {
    pVtRegs[14] -= sizeof(DWORD_PTR);
    *(PDWORD_PTR)(pVtRegs[14]) = *(PDWORD_PTR)(opAddr1);
}

void Pop(DWORD_PTR opAddr1, PDWORD_PTR pVtRegs) {
    *(PDWORD_PTR)(opAddr1) = *(PDWORD_PTR)(pVtRegs[14]);
    pVtRegs[14] += sizeof(DWORD_PTR);
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

void Imul(char opType1, char opBit1, DWORD_PTR opAddr1, char opBit2, DWORD_PTR opAddr2) {
    switch (opBit1)
    {
    case 'q':
        *(PDWORD64)opAddr1 *= *(PDWORD64)opAddr2;
        break;
    case 'd':
        if (opType1 == 'r') {
            *(PDWORD_PTR)opAddr1 = *(PDWORD)opAddr1 * *(PDWORD)opAddr2;
        }
        else {
            *(PDWORD)opAddr1 *= *(PDWORD)opAddr2;
        }
        break;
    case 'w':
        *(PWORD)opAddr1 *= *(PWORD)opAddr2;
        break;
    case 'b':
        *(PBYTE)opAddr1 *= *(PBYTE)opAddr2;
        break;
    }
}

void And(char opType1, char opBit1, DWORD_PTR opAddr1, char opBit2, DWORD_PTR opAddr2) {
    switch (opBit1)
    {
    case 'q':
        *(PDWORD64)opAddr1 &= *(PDWORD64)opAddr2;
        break;
    case 'd':
        if (opType1 == 'r') {
            *(PDWORD_PTR)opAddr1 = *(PDWORD)opAddr1 & *(PDWORD)opAddr2;
        }
        else {
            *(PDWORD)opAddr1 &= *(PDWORD)opAddr2;
        }
        break;
    case 'w':
        *(PWORD)opAddr1 &= *(PWORD)opAddr2;
        break;
    case 'b':
        *(PBYTE)opAddr1 &= *(PBYTE)opAddr2;
        break;
    }
}

void Or(char opType1, char opBit1, DWORD_PTR opAddr1, char opBit2, DWORD_PTR opAddr2) {
    switch (opBit1)
    {
    case 'q':
        *(PDWORD64)opAddr1 |= *(PDWORD64)opAddr2;
        break;
    case 'd':
        if (opType1 == 'r') {
            *(PDWORD_PTR)opAddr1 = *(PDWORD)opAddr1 | *(PDWORD)opAddr2;
        }
        else {
            *(PDWORD)opAddr1 |= *(PDWORD)opAddr2;
        }
        break;
    case 'w':
        *(PWORD)opAddr1 |= *(PWORD)opAddr2;
        break;
    case 'b':
        *(PBYTE)opAddr1 |= *(PBYTE)opAddr2;
        break;
    }
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

void Shl(char opType1, char opBit1, DWORD_PTR opAddr1, char opBit2, DWORD_PTR opAddr2) {
    switch (opBit1)
    {
    case 'q':
        *(PDWORD64)opAddr1 <<= *(PDWORD64)opAddr2;
        break;
    case 'd':
        if (opType1 == 'r') {
            *(PDWORD_PTR)opAddr1 = *(PDWORD)opAddr1 << *(PDWORD)opAddr2;
        }
        else {
            *(PDWORD)opAddr1 <<= *(PDWORD)opAddr2;
        }
        break;
    case 'w':
        *(PWORD)opAddr1 <<= *(PWORD)opAddr2;
        break;
    case 'b':
        *(PBYTE)opAddr1 <<= *(PBYTE)opAddr2;
        break;
    }
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

void RepMovsb(PDWORD_PTR pVtRegs) {
    while (pVtRegs[2]) { // vtRCX
        *(PBYTE)pVtRegs[5] = *(PBYTE)pVtRegs[4]; // byte ptr [vtRDI] = byte ptr [vtRSI]
        pVtRegs[4]++;
        pVtRegs[5]++;
        pVtRegs[2]--;
    }
}

int Jmp(DWORD_PTR vtEFL) {
    return 1;
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
    PVOID mnemonicMap[] = { Push, Pop, AsmCall, Ret, Mov, AsmMovsx, AsmMovzx, Movsxd, Lea, Add, Inc, Sub, AsmCmp, Imul, And, Or, Xor, AsmTest, Shl, Cdqe, RepStosb, RepMovsb, Jmp, AsmJe, AsmJne, AsmJbe, AsmJl, AsmJge, AsmJle };
    PVOID instructionFunc = mnemonicMap[mnemonicIndex];
    DWORD_PTR vtRIP = pVtRegs[16];

    // 调用指令函数
    // Push - Ret
    if (mnemonicIndex < 4) {
        ((void(*)(...))instructionFunc)(opAddr1, pVtRegs);
    }
    else if (instructionFunc == AsmMovsx || instructionFunc == AsmMovzx) {
        ((void(*)(...))instructionFunc)(opBit1, opAddr1, opBit2, opAddr2);
    }
    else if (instructionFunc == AsmCmp || instructionFunc == AsmTest) {
        ((void(*)(...))instructionFunc)(opBit1, opAddr1, opAddr2, pVtRegs);
    }
    else if (instructionFunc == Cdqe || instructionFunc == RepStosb || instructionFunc == RepMovsb) {
        ((void(*)(...))instructionFunc)(pVtRegs);
    }
    // Jcc
    else if (mnemonicIndex > 21) {
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