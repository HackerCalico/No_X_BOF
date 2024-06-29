#include <iostream>
#include <windows.h>

#include "FuncType.h"
#include "Instruction.h"

using namespace std;

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
    int selfAsmLength = ((pStrlen)pFuncAddr[3])(selfAsm);
    for (int i = 0; i < selfAsmLength; i++) {
        if (selfAsm[i] == '_') {
            *(selfAsm + i) = '\0';
        }
    }

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
    // 自定义汇编 (CMD 命令执行)
    char selfAsm[] = "0_4_q_pq70+i20_q_q38_5_4_q_pq70+i18_q_q30_a_4_d_pq70+i10_d_d18_e_4_q_pq70+i8_q_q10_13_0_q_q28_q__14_11_q_q70_q_i120_1b_4_d_d0_d_i8_20_13_q_q0_q_i0_24_4_d_d10_d_i96_29_4_q_q18_q_pq70+i150_31_2_q_pq18+q0_q__34_4_q_q10_q_pq70+i140_3c_4_q_pq10_q_q0_3f_4_q_q0_q_pq70+i140_47_4_q_q0_q_pq0_4a_4_b_pq0_b_i30_4d_4_q_q0_q_pq70+i140_55_4_q_q0_q_pq0_58_4_b_pq0+i1_b_i0_5c_4_q_q0_q_pq70+i148_64_4_d_pq0_d_i1_6a_4_d_pq70+i78_d_i18_72_4_d_pq70+i88_d_i1_7d_4_q_pq70+i80_q_i0_89_4_d_d0_d_i8_8e_13_q_q0_q_i9_92_16_d_d38_d_d38_95_8_q_q30_q_lq70+i78_9a_8_q_q18_q_lq70+i58_9f_8_q_q10_q_lq70+i60_a4_4_q_q40_q_pq70+i150_ac_2_q_pq40+q0_q__b0_17_d_d0_d_d0_b2_23_q_ib9_q__b4_21_q_i2e5_q__b9_8_q_q0_q_lq70+ib0_c1_4_q_q28_q_q0_c4_16_d_d0_d_d0_c6_4_d_d10_d_i68_cb_20_b_pq28_b_b0_cd_4_d_pq70+ib0_d_i68_d8_4_q_q0_q_pq70+i58_dd_4_q_pq70+i110_q_q0_e5_4_q_q0_q_pq70+i58_ea_4_q_pq70+i108_q_q0_f2_16_d_d0_d_d0_f4_4_w_pq70+if0_w_w0_fc_4_d_pq70+iec_d_i101_107_4_d_d0_d_i8_10c_13_q_q0_q_i8_110_8_q_q10_q_lq70+i90_118_4_q_pq70+i48_q_q10_11d_8_q_q10_q_lq70+ib0_125_4_q_pq70+i40_q_q10_12a_4_q_pq70+i38_q_i0_133_4_q_pq70+i30_q_i0_13c_4_d_pq70+i28_d_i0_144_4_d_pq70+i20_d_i1_14c_16_d_d38_d_d38_14f_16_d_d30_d_d30_152_4_q_q18_q_pq70+i130_15a_16_d_d10_d_d10_15c_4_q_q28_q_pq70+i150_164_2_q_pq28+q0_q__167_17_d_d0_d_d0_169_23_q_i1a2_q__16b_4_d_d0_d_i8_170_13_q_q0_q_i7_174_4_q_q10_q_pq70+i60_179_4_q_q18_q_pq70+i150_181_2_q_pq18+q0_q__184_4_d_d0_d_i8_189_13_q_q0_q_i7_18d_4_q_q10_q_pq70+i58_192_4_q_q18_q_pq70+i150_19a_2_q_pq18+q0_q__19d_21_q_i2e5_q__1a2_4_d_d0_d_i8_1a7_13_q_q0_q_i7_1ab_4_q_q10_q_pq70+i58_1b0_4_q_q18_q_pq70+i150_1b8_2_q_pq18+q0_q__1bb_4_d_d0_d_i8_1c0_13_q_q0_q_i7_1c4_4_q_q10_q_pq70+i98_1cc_4_q_q18_q_pq70+i150_1d4_2_q_pq18+q0_q__1d7_4_d_d0_d_i8_1dc_13_q_q0_q_i7_1e0_4_q_q10_q_pq70+i90_1e8_4_q_q18_q_pq70+i150_1f0_2_q_pq18+q0_q__1f3_4_q_q0_q_pq70+i148_1fb_4_d_pq0_d_i0_201_4_q_q0_q_pq70+i148_209_7_q_q0_d_pq0_20c_4_q_q10_q_pq70+i140_214_9_q_q0_q_pq10_217_4_d_d10_d_i8_21c_13_q_q10_q_ia_220_4_q_pq70+i68_q_q10_225_4_q_pq70+i20_q_i0_22e_8_q_q38_q_lq70+i50_233_4_d_d30_d_i64_239_4_q_q18_q_q0_23c_4_q_q10_q_pq70+i60_241_4_q_q0_q_pq70+i150_249_4_q_q28_q_pq70+i68_24e_2_q_pq0+q28_q__251_17_d_d0_d_d0_253_22_q_i2cc_q__255_12_d_pq70+i50_d_i0_25a_22_q_i2cc_q__25c_4_q_q0_q_pq70+i148_264_4_d_d0_d_pq0_266_9_d_d0_d_pq70+i50_26a_4_q_q10_q_pq70+i148_272_4_d_pq10_d_d0_274_4_q_q0_q_pq70+i148_27c_12_d_pq0_d_i64_27f_25_q_i2c7_q__281_4_q_q0_q_pq70+i148_289_4_d_d0_d_pq0_28b_9_d_d0_d_i64_28e_19_q__q__290_4_d_d10_d_i8_295_13_q_q10_q_i1_299_4_q_pq70+i70_q_q10_29e_4_q_q18_q_q0_2a1_4_q_q0_q_pq70+i140_2a9_4_q_q10_q_pq0_2ac_4_q_q0_q_pq70+i150_2b4_4_q_q28_q_pq70+i70_2b9_2_q_pq0+q28_q__2bc_4_q_q10_q_pq70+i140_2c4_4_q_pq10_q_q0_2c7_21_q_i201_q__2cc_4_d_d0_d_i8_2d1_13_q_q0_q_i7_2d5_4_q_q10_q_pq70+i60_2da_4_q_q18_q_pq70+i150_2e2_2_q_pq18+q0_q__2e5_9_q_q70_q_i120_2ec_1_q_q28_q__2ed_3_q__q__!";

    // ShellCode 参数 (CMD 命令执行)
    char commandPara[] = "cmd /c tasklist";
    int commandParaLength = strlen(commandPara) + 1;
    char* outputData;
    int outputDataLength;
    PVOID funcAddr[] = { malloc, realloc, free, strlen, strtol, ((errno_t(*)(char*, rsize_t, const char*))strcpy_s), ((int(*)(char*, size_t, const char*, ...))sprintf_s), CloseHandle, CreateProcessA, CreatePipe, ReadFile, FindFirstFileA, FindNextFileA, FindClose, GetFullPathNameA, FileTimeToSystemTime };

    // 调用解释器
    MagicInvoke(selfAsm, commandPara, commandParaLength, &outputData, &outputDataLength, funcAddr);

    // ShellCode 输出
    cout << outputData;
}