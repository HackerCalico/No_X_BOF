#include <iostream>
#include <windows.h>

#include "Instruction.h"

using namespace std;

// 计算 (未考虑“*”)
DWORD_PTR calculate(DWORD_PTR number1, DWORD_PTR number2, char symbol) {
    switch (symbol)
    {
    case '+':
        return number1 + number2;
    case '-':
        return number1 - number2;
    }
}

// 解析算式 (未考虑“*”)
void ParseFormula(char* op, char** pFormula, char** symbols) {
    int n = 1;
    int opLength = strlen(op);
    for (int i = 0; i < opLength; i++) {
        switch (op[i])
        {
        case '+':
            pFormula[n] = symbols[0];
            *(op + i) = '\0';
            n += 2;
            break;
        case '-':
            pFormula[n] = symbols[1];
            *(op + i) = '\0';
            n += 2;
            break;
        }
    }
    n = 0;
    for (int i = 0; i < opLength; i += strlen(op + i) + 1) {
        pFormula[n] = op + i;
        n += 2;
    }
}

// 获取操作数值的 类型(r寄存器/m内存空间) + 地址
DWORD_PTR GetOpTypeAndAddr(char* op, char* pOpType1, PDWORD_PTR pVtRegs, PDWORD_PTR opNumber) {
    char* endPtr;
    char tempOp[50] = "";
    char* symbols[] = { (char*)"+", (char*)"-" };
    char* formula[] = { NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL };

    // 立即数
    if (op[0] == 'i') {
        *opNumber = strtol(op + 1, &endPtr, 16);
        return (DWORD_PTR)opNumber;
    }
    // lea [] / ptr []
    else if (op[0] == 'l' || op[0] == 'p') {
        // 拷贝操作数 (解析算式过程会导致变化)
        strcpy_s(tempOp, sizeof(tempOp), op);
        // 解析算式 (未考虑“*”)
        ParseFormula(op + 1, formula, symbols);
        // 计算 (未考虑“*”)
        DWORD_PTR number1 = 0;
        DWORD_PTR number2;
        char symbol = '+';
        for (int i = 0; formula[i] != NULL; i++) {
            switch (formula[i][0])
            {
            case 'i':
                DWORD_PTR tempNumber; // 存储数字
                GetOpTypeAndAddr(formula[i], NULL, pVtRegs, &tempNumber);
                number1 = calculate(number1, tempNumber, symbol);
                break;
            case 'q':
                number2 = *(PDWORD64)GetOpTypeAndAddr(formula[i], NULL, pVtRegs, NULL);
                number1 = calculate(number1, number2, symbol);
                break;
            case 'd':
                number2 = *(PDWORD)GetOpTypeAndAddr(formula[i], NULL, pVtRegs, NULL);
                number1 = calculate(number1, number2, symbol);
                break;
            case 'w':
                number2 = *(PWORD)GetOpTypeAndAddr(formula[i], NULL, pVtRegs, NULL);
                number1 = calculate(number1, number2, symbol);
                break;
            case 'b':
                number2 = *(PBYTE)GetOpTypeAndAddr(formula[i], NULL, pVtRegs, NULL);
                number1 = calculate(number1, number2, symbol);
                break;
            case '+':
                symbol = '+';
                break;
            case '-':
                symbol = '-';
                break;
            }
        }
        // 还原操作数
        strcpy_s(op, sizeof(tempOp), tempOp);
        // lea []
        if (op[0] == 'l') {
            *opNumber = number1;
            return (DWORD_PTR)opNumber;
        }
        // ptr []
        if (pOpType1 != NULL) {
            *pOpType1 = 'm';
        }
        return number1;
    }
    // 寄存器
    else {
        if (pOpType1 != NULL) {
            *pOpType1 = 'r';
        }
        return (DWORD_PTR)pVtRegs + strtol(op + 1, &endPtr, 16);
    }
}

// 解析自定义汇编
void ParseSelfAsm(char* selfAsm, PDWORD_PTR pVtRegs) {
    int selfAsmLength = strlen(selfAsm);
    for (int i = 0; i < selfAsmLength; i++) {
        if (selfAsm[i] == '_') {
            *(selfAsm + i) = '\0';
        }
    }

    // 逐条解析
    int i = 0;
    int num = 0;
    char* endPtr;
    int* vtAddrMapping = (int*)malloc(1000 * sizeof(int*)); // num -> 虚拟地址
    int* numMapping = (int*)malloc(3000 * sizeof(int*)); // 虚拟地址 -> num
    int* mnemonicIndexMapping = (int*)malloc(1000 * sizeof(int*)); // num -> mnemonicIndex
    char** opBit1Mapping = (char**)malloc(1000 * sizeof(char*)); // num -> opBit1
    char** op1Mapping = (char**)malloc(1000 * sizeof(char*)); // num -> op1
    char** opBit2Mapping = (char**)malloc(1000 * sizeof(char*)); // num -> opBit2
    char** op2Mapping = (char**)malloc(1000 * sizeof(char*)); // num -> op2
    while (selfAsm[i] != '!') {
        // 虚拟地址
        DWORD_PTR vtAddr = strtol(selfAsm + i, &endPtr, 16);
        i += strlen(selfAsm + i) + 1;
        vtAddrMapping[num] = vtAddr;
        numMapping[vtAddr] = num;

        // 助记符序号
        mnemonicIndexMapping[num] = atoi(selfAsm + i);
        i += strlen(selfAsm + i) + 1;

        // 操作数1 位数
        opBit1Mapping[num] = selfAsm + i;
        i += strlen(selfAsm + i) + 1;

        // 操作数1
        op1Mapping[num] = selfAsm + i;
        i += strlen(selfAsm + i) + 1;

        // 操作数2 位数
        opBit2Mapping[num] = selfAsm + i;
        i += strlen(selfAsm + i) + 1;

        // 操作数1
        op2Mapping[num] = selfAsm + i;
        i += strlen(selfAsm + i) + 1;

        num++;
    }

    // 逐条执行
    for (int i = 0; i < num; i++) {
        pVtRegs[16] = vtAddrMapping[i];
        int mnemonicIndex = mnemonicIndexMapping[i];
        char* opBit1 = opBit1Mapping[i];
        char* op1 = op1Mapping[i];
        char* opBit2 = opBit2Mapping[i];
        char* op2 = op2Mapping[i];

        // 获取两个操作数的 类型(r寄存器/m内存空间) + 地址
        char opType1;
        DWORD_PTR opAddr1 = NULL;
        DWORD_PTR opAddr2 = NULL;
        DWORD_PTR opNumber; // 存储数字
        if (strlen(op1)) {
            opAddr1 = GetOpTypeAndAddr(op1, &opType1, pVtRegs, &opNumber);
        }
        if (strlen(op2)) {
            opAddr2 = GetOpTypeAndAddr(op2, NULL, pVtRegs, &opNumber);
        }

        // 调用指令
        if (InvokeInstruction(mnemonicIndex, opType1, opBit1[0], opAddr1, opBit2[0], opAddr2, pVtRegs)) {
            i = numMapping[pVtRegs[16]]; // Jcc 指令跳转
            i--;
        }
    }

    free(vtAddrMapping);
    free(numMapping);
    free(mnemonicIndexMapping);
    free(opBit1Mapping);
    free(op1Mapping);
    free(opBit2Mapping);
    free(op2Mapping);
}

// 魔法调用
void MagicInvoke(char* selfAsm, char* commandPara, int commandParaLength, char** pOutputData, int* pOutputDataLength, PVOID* pFuncAddr) {
    // 创建虚拟栈
    PVOID vtStack = malloc(0x10000);

    // 创建虚拟寄存器
    /*
    * 0  RAX
    * 1  RBX
    * 2  RCX
    * 3  RDX
    * 4  RSI
    * 5  RDI
    * 6  R8
    * 7  R9
    * 8  R10
    * 9  R11
    * 10 R12
    * 11 R13
    * 12 R14
    * 13 R15
    * 14 RSP
    * 15 RBP
    * 16 RIP
    * 17 EFL
    */
    DWORD_PTR vtRegs[18] = { 0 };
    vtRegs[14] = vtRegs[15] = (DWORD_PTR)vtStack + 0x9000;

    // 设置虚拟寄存器的初值
    /*
    * ShellCode(commandPara, commandParaLength, &outputData, &outputDataLength, funcAddr);
    * lea rax, [funcAddr]
    * mov qword ptr [rsp+20h], rax
    * lea r9, [outputDataLength]
    * lea r8, [outputData]
    * mov edx, dword ptr [commandParaLength]
    * lea rcx, [commandPara]
    * call ShellCode
    */
    vtRegs[0] = (DWORD_PTR)pFuncAddr;
    *(PDWORD_PTR)(vtRegs[14] + 0x20) = vtRegs[0];
    vtRegs[7] = (DWORD_PTR)pOutputDataLength;
    vtRegs[6] = (DWORD_PTR)pOutputData;
    vtRegs[3] = commandParaLength;
    vtRegs[2] = (DWORD_PTR)commandPara;
    vtRegs[14] = vtRegs[14] - sizeof(DWORD_PTR);

    // 解析自定义汇编
    ParseSelfAsm(selfAsm, vtRegs);
    free(vtStack);
}