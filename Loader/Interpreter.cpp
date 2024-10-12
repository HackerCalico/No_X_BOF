#include "Interpreter.h"

PVOID pVtStack = NULL;

int dllLoadNum = 0;
int* dllHashList = NULL;
HMODULE* dllAddrList = NULL;

int winApiGetNum = 0;
int* winApiHashList = NULL;
PVOID* winApiPtrList = NULL;

__declspec(noinline) int GetHash(char* string, int length);
__declspec(noinline) DWORD_PTR Calculate(DWORD_PTR number1, DWORD_PTR number2, char symbol);
__declspec(noinline) DWORD_PTR GetOpTypeAndAddr(char* op, char* pOpType1, DWORD_PTR vtRegs[], DWORD_PTR* pOpNumber);
__declspec(noinline) int TextReloc(char* op, char* textRelocNames, PVOID pRdata, int& relocIndex);
__declspec(noinline) int InvokeInstruction(BYTE mnemonicIndex, char opType1, char opBit1, DWORD_PTR opAddr1, char opBit2, DWORD_PTR opAddr2, DWORD_PTR vtRegs[]);

#pragma code_seg(".shell")

int RunPayload(PBYTE pPayload, int payloadSize, int bofFuncHash, char* commandPara, int commandParaLen, char*& outputData, int& outputDataLen, PVOID specialParaList[]) {    
    // 解析 Payload
    if (payloadSize < sizeof(WORD)) {
        return 0;
    }
    WORD bofFuncOffsetMapLen = *(PWORD)pPayload;
    PBYTE pBofFuncOffsetMap = pPayload + sizeof(WORD);
    if (payloadSize < sizeof(WORD) * 2 + bofFuncOffsetMapLen) {
        return 0;
    }
    WORD textRelocNamesLen = *(PWORD)(pBofFuncOffsetMap + bofFuncOffsetMapLen);
    char* textRelocNames = (char*)pBofFuncOffsetMap + bofFuncOffsetMapLen + sizeof(WORD);
    if (payloadSize < sizeof(WORD) * 3 + bofFuncOffsetMapLen + textRelocNamesLen) {
        return 0;
    }
    WORD selfAsmLen = *(PWORD)(textRelocNames + textRelocNamesLen);
    char* selfAsm = textRelocNames + textRelocNamesLen + sizeof(WORD);
    if (payloadSize < sizeof(WORD) * 3 + bofFuncOffsetMapLen + textRelocNamesLen + selfAsmLen) {
        return 0;
    }
    PVOID pRdata = selfAsm + selfAsmLen;

    // 查找 BOF 函数虚拟地址
    WORD bofFuncVtAddr = 0xFFFF;
    for (int i = 0; i < bofFuncOffsetMapLen; i += 6) {
        if (*(int*)(pBofFuncOffsetMap + i) == bofFuncHash) {
            bofFuncVtAddr = *(PWORD)(pBofFuncOffsetMap + i + sizeof(int));
            break;
        }
    }
    if (bofFuncVtAddr == 0xFFFF) {
        return 0;
    }

    // .text 重定位
    int relocIndex = 0;
    for (int offset = 0; offset < selfAsmLen;) {
        offset += 4;
        char* op1 = selfAsm + offset;
        offset += strlen(op1) + 2;
        char* op2 = selfAsm + offset;
        offset += strlen(op2) + 1;
        if (!TextReloc(op1, textRelocNames, pRdata, relocIndex)) {
            return 0;
        }
        if (!TextReloc(op2, textRelocNames, pRdata, relocIndex)) {
            return 0;
        }
    }

    // 创建虚拟栈
    if (pVtStack == NULL) {
        pVtStack = malloc(0x10000);
    }
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
    DWORD_PTR vtRegs[18];
    vtRegs[14] = vtRegs[15] = (DWORD_PTR)pVtStack + 0x9000;
    // 初始化虚拟寄存器
    /*
    * BofFunc(commandPara, commandParaLen, &outputData, &outputDataLen, specialParaList);
    * lea rax, [specialParaList]
    * mov qword ptr [rsp+20h], rax
    * lea r9, [outputDataLen]
    * lea r8, [outputData]
    * mov edx, dword ptr [commandParaLen]
    * lea rcx, [commandPara]
    * call BofFunc
    */
    vtRegs[0] = (DWORD_PTR)specialParaList;
    *(PDWORD_PTR)(vtRegs[14] + 0x20) = vtRegs[0];
    vtRegs[7] = (DWORD_PTR)&outputDataLen;
    vtRegs[6] = (DWORD_PTR)&outputData;
    vtRegs[3] = commandParaLen;
    vtRegs[2] = (DWORD_PTR)commandPara;
    vtRegs[14] -= sizeof(DWORD_PTR);
    *(PDWORD_PTR)vtRegs[14] = 0xFFFFFFFFFFFFFFFF;

    // 运行 BOF 函数
    WORD targetVtAddr = 0xFFFF;
    for (int offset = 0; offset < selfAsmLen;) {
        WORD vtAddr = *(PWORD)(selfAsm + offset);
        offset += sizeof(WORD);
        if (vtAddr == bofFuncVtAddr) {
            targetVtAddr = vtAddr;
            bofFuncVtAddr = 0xFFFF;
        }

        BYTE mnemonicIndex = *(PBYTE)(selfAsm + offset);
        offset += sizeof(BYTE);
        char opBit1 = *(char*)(selfAsm + offset);
        offset += sizeof(char);
        char* op1 = selfAsm + offset;
        offset += strlen(op1) + 1;
        char opBit2 = *(char*)(selfAsm + offset);
        offset += sizeof(char);
        char* op2 = selfAsm + offset;
        offset += strlen(op2) + 1;

        if (vtAddr >= targetVtAddr) {
            vtRegs[16] = vtAddr;
            // 获取两个操作数的 Type(i立即数/r寄存器/m内存空间) 和 地址
            char opType1 = NULL;
            DWORD_PTR opAddr1 = NULL;
            DWORD_PTR opAddr2 = NULL;
            DWORD_PTR opNumber = NULL;
            if (*op1 != '\0') {
                opAddr1 = GetOpTypeAndAddr(op1, &opType1, vtRegs, &opNumber);
                if (opAddr1 == NULL || opType1 == NULL) {
                    return 0;
                }
            }
            else if (*op2 != '\0') {
                return 0;
            }
            if (*op2 != '\0') {
                opAddr2 = GetOpTypeAndAddr(op2, NULL, vtRegs, &opNumber);
                if (opAddr2 == NULL) {
                    return 0;
                }
            }
            // 调用指令
            // printf("vtAddr: 0x%llx, mnemonicIndex: %d, opBit1: %c, op1: %s, opBit2: %c, op2: %s\n", vtAddr, mnemonicIndex, opBit1, op1, opBit2, op2);
            int isJmp = InvokeInstruction(mnemonicIndex, opType1, opBit1, opAddr1, opBit2, opAddr2, vtRegs);
            if (isJmp == 2) {
                return 0;
            }
            else if (isJmp) {
                if (vtRegs[16] == 0xFFFFFFFFFFFFFFFF) {
                    return GetHash(selfAsm, selfAsmLen);
                }
                targetVtAddr = vtRegs[16];
                offset = 0;
            }
            // printf("RAX: 0x%llx\nRBX: 0x%llx\nRCX: 0x%llx\nRDX: 0x%llx\nRSI: 0x%llx\nRDI: 0x%llx\nR8: 0x%llx\nR9: 0x%llx\nR10: 0x%llx\nR11: 0x%llx\nR12: 0x%llx\nR13: 0x%llx\nR14: 0x%llx\nR15: 0x%llx\nRSP: 0x%llx\nRBP: 0x%llx\nRIP: 0x%llx\nELF: 0x%llx\n", vtRegs[0], vtRegs[1], vtRegs[2], vtRegs[3], vtRegs[4], vtRegs[5], vtRegs[6], vtRegs[7], vtRegs[8], vtRegs[9], vtRegs[10], vtRegs[11], vtRegs[12], vtRegs[13], vtRegs[14], vtRegs[15], vtRegs[16], vtRegs[17]);
            // printf("\n");
        }
    }
    return 0;
}

__declspec(noinline) int GetHash(char* string, int length) {
    int hash = 0;
    for (int i = 0; i < length; i++) {
        hash += string[i];
        hash = (hash << 8) - hash;
    }
    return hash;
}

// 计算 (未考虑“*”)
__declspec(noinline) DWORD_PTR Calculate(DWORD_PTR number1, DWORD_PTR number2, char symbol) {
    if (symbol == '+') {
        return number1 + number2;
    }
    else {
        return number1 - number2;
    }
}

// 获取操作数值的 类型(i立即数/r寄存器/m内存空间) 和 地址
__declspec(noinline) DWORD_PTR GetOpTypeAndAddr(char* op, char* pOpType1, DWORD_PTR vtRegs[], DWORD_PTR* pOpNumber) {
    // 立即数
    if (*op == 'i') {
        if (pOpNumber == NULL) {
            return NULL;
        }
        char* endPtr;
        *pOpNumber = strtoull(op + 1, &endPtr, 16);
        if (*endPtr != '\0') {
            return NULL;
        }
        if (pOpType1 != NULL) {
            *pOpType1 = 'i';
        }
        return (DWORD_PTR)pOpNumber;
    }
    // lea [] / ptr []
    else if (*op == 'l' || *op == 'p') {
        // 计算 (未考虑“*”)
        int formulaLen = 1;
        while (op[formulaLen] != '\0') {
            if (op[formulaLen] == '+' || op[formulaLen] == '-') {
                op[formulaLen] = '\0';
                formulaLen++;
            }
            formulaLen++;
        }
        char symbol = '+';
        DWORD_PTR number1 = 0;
        DWORD_PTR number2 = NULL;
        DWORD_PTR tempNumber = NULL;
        for (int i = 1; i < formulaLen;) {
            if (op[i] == 'i' || op[i] == 'q' || op[i] == 'd' || op[i] == 'w' || op[i] == 'b') {
                number2 = GetOpTypeAndAddr(op + i, NULL, vtRegs, &tempNumber);
                if (number2 == NULL) {
                    return NULL;
                }
                if (op[i] == 'i') {
                    number1 = Calculate(number1, *(PDWORD_PTR)number2, symbol);
                }
                else if (op[i] == 'q') {
                    number1 = Calculate(number1, *(PDWORD64)number2, symbol);
                }
                else if (op[i] == 'd') {
                    number1 = Calculate(number1, *(PDWORD)number2, symbol);
                }
                else if (op[i] == 'w') {
                    number1 = Calculate(number1, *(PWORD)number2, symbol);
                }
                else if (op[i] == 'b') {
                    number1 = Calculate(number1, *(PBYTE)number2, symbol);
                }
                i += strlen(op + i) + 1;
            }
            else if (op[i] == '+' || op[i] == '-') {
                symbol = op[i];
                op[i - 1] = symbol;
                i++;
            }
            else {
                return NULL;
            }
        }
        // lea []
        if (*op == 'l') {
            if (pOpNumber == NULL) {
                return NULL;
            }
            *pOpNumber = number1;
            return (DWORD_PTR)pOpNumber;
        }
        // ptr []
        if (pOpType1 != NULL) {
            *pOpType1 = 'm';
        }
        return number1;
    }
    // 寄存器
    else if (*op == 'q' || *op == 'd' || *op == 'w' || *op == 'b') {
        char* endPtr;
        DWORD_PTR offset = strtoull(op + 1, &endPtr, 16);
        if (*endPtr != '\0') {
            return NULL;
        }
        if (pOpType1 != NULL) {
            *pOpType1 = 'r';
        }
        return (DWORD_PTR)vtRegs + offset;
    }
    return NULL;
}

__declspec(noinline) int TextReloc(char* op, char* textRelocNames, PVOID pRdata, int& relocIndex) {
    if (dllHashList == NULL) {
        dllHashList = (int*)malloc(1000 * sizeof(int));
        dllAddrList = (HMODULE*)malloc(1000 * sizeof(HMODULE));
        winApiHashList = (int*)malloc(1000 * sizeof(int));
        winApiPtrList = (PVOID*)malloc(1000 * sizeof(PVOID));
    }
    else if (dllLoadNum >= 1000 || winApiGetNum >= 1000) {
        return 0;
    }
    char* rip = strchr(op, 'r'); // r0000000000000000
    if (rip != NULL && *(PDWORD_PTR)(rip + 1) == 0x3030303030303030) {
        char end = rip[17];
        for (int count = 0; count < relocIndex; textRelocNames++) {
            if (*textRelocNames == '\0') {
                count++;
            }
        }
        char* format = "i%016llx";
        if (GetHash(textRelocNames, strlen(textRelocNames)) == -1394134574) { // .rdata
            sprintf_s(rip, 18, format, (DWORD_PTR)pRdata);
        }
        else {
            char* dollar = strchr(textRelocNames, '$');
            if (dollar != NULL) {
                *dollar = '\0';
                // DLL
                int isExist = 0;
                HMODULE hDll = NULL;
                int dllHash = GetHash(textRelocNames, strlen(textRelocNames));
                for (int i = 0; i < dllLoadNum; i++) {
                    if (dllHashList[i] == dllHash) {
                        isExist = 1;
                        hDll = dllAddrList[i];
                        break;
                    }
                }
                if (!isExist) {
                    if (dllHash == -1499897628) { // Kernel32
                        hDll = GetModuleHandleA(textRelocNames);
                    }
                    else {
                        hDll = LoadLibraryA(textRelocNames);
                    }
                    dllHashList[dllLoadNum] = dllHash;
                    dllAddrList[dllLoadNum] = hDll;
                    dllLoadNum++;
                }
                if (hDll == NULL) {
                    return 0;
                }
                // Windows Api
                isExist = 0;
                PVOID pWinApi;
                int winApiHash = GetHash(dollar + 1, strlen(dollar + 1));
                for (int i = 0; i < winApiGetNum; i++) {
                    if (winApiHashList[i] == winApiHash) {
                        isExist = 1;
                        pWinApi = winApiPtrList[i];
                        break;
                    }
                }
                if (!isExist) {
                    DWORD_PTR winApiAddr = (DWORD_PTR)GetProcAddress(hDll, dollar + 1);
                    if (winApiAddr == 0) {
                        return 0;
                    }
                    pWinApi = malloc(sizeof(DWORD_PTR));
                    *(PDWORD_PTR)pWinApi = winApiAddr;
                    winApiHashList[winApiGetNum] = winApiHash;
                    winApiPtrList[winApiGetNum] = pWinApi;
                    winApiGetNum++;
                }
                *dollar = '$';
                sprintf_s(rip, 18, format, (DWORD_PTR)pWinApi);
            }
            else {
                return 0;
            }
        }
        rip[17] = end;
        relocIndex++;
    }
    return 1;
}

__declspec(noinline) int InvokeInstruction(BYTE mnemonicIndex, char opType1, char opBit1, DWORD_PTR opAddr1, char opBit2, DWORD_PTR opAddr2, DWORD_PTR vtRegs[]) {
    const int pushIndex = 0, popIndex = 1, callIndex = 2, retIndex = 3, movzxIndex = 4, movsxdIndex = 5, cmpIndex = 6, testIndex = 7, shlIndex = 8, shrIndex = 9, nopIndex = 10, movIndex = 11, movabsIndex = 12, leaIndex = 13, addIndex = 14, incIndex = 15, subIndex = 16, decIndex = 17, andIndex = 18, orIndex = 19, xorIndex = 20, jmpIndex = 21, jeIndex = 22, jneIndex = 23, jbeIndex = 24, jlIndex = 25, jgeIndex = 26, jleIndex = 27;
    if (mnemonicIndex < 0 || mnemonicIndex > 27) {
        return 2;
    }
    DWORD_PTR vtRIP = vtRegs[16];

    if (mnemonicIndex >= jmpIndex) {
        __asm {
            mov r14, 0x01
            mov r15, qword ptr[vtRegs]
            mov r15, qword ptr[r15 + 0x88] // vtEFL
        }
        if (mnemonicIndex == jeIndex) {
            __asm {
                push r15
                popf
                je isJmp
                mov r14, 0x00
                isJmp:
            }
        }
        else if (mnemonicIndex == jneIndex) {
            __asm {
                push r15
                popf
                jne isJmp
                mov r14, 0x00
                isJmp:
            }
        }
        else if (mnemonicIndex == jbeIndex) {
            __asm {
                push r15
                popf
                jbe isJmp
                mov r14, 0x00
                isJmp:
            }
        }
        else if (mnemonicIndex == jlIndex) {
            __asm {
                push r15
                popf
                jl isJmp
                mov r14, 0x00
                isJmp:
            }
        }
        else if (mnemonicIndex == jgeIndex) {
            __asm {
                push r15
                popf
                jge isJmp
                mov r14, 0x00
                isJmp:
            }
        }
        else if (mnemonicIndex == jleIndex) {
            __asm {
                push r15
                popf
                jle isJmp
                mov r14, 0x00
                isJmp:
            }
        }
        __asm {
            cmp r14, 0x01
            jne notJmp
            mov rax, qword ptr[opAddr1]
            mov rcx, qword ptr[rax]
            mov rax, qword ptr[vtRegs]
            mov qword ptr[rax + 0x80], rcx // vtRIP
            notJmp:
        }
    }
    else if (mnemonicIndex > nopIndex) {
        if (opAddr1 == NULL) {
            return 2;
        }
        if (opAddr2 != NULL) {
            __asm {
                mov r12, qword ptr[opAddr2]
                mov r12, qword ptr[r12]
            }
        }
        __asm {
            mov r10, qword ptr[opAddr1]
            mov r11, qword ptr[r10]
        }
        if (mnemonicIndex == movIndex || mnemonicIndex == movabsIndex || mnemonicIndex == leaIndex) {
            __asm {
                mov r11, r12
            }
        }
        else if (mnemonicIndex == addIndex) {
            __asm {
                add r11, r12
            }
        }
        else if (mnemonicIndex == incIndex) {
            __asm {
                inc r11
            }
        }
        else if (mnemonicIndex == subIndex) {
            __asm {
                sub r11, r12
            }
        }
        else if (mnemonicIndex == decIndex) {
            __asm {
                dec r11
            }
        }
        else if (mnemonicIndex == andIndex) {
            __asm {
                and r11, r12
            }
        }
        else if (mnemonicIndex == orIndex) {
            __asm {
                or r11, r12
            }
        }
        else if (mnemonicIndex == xorIndex) {
            __asm {
                xor r11, r12
            }
        }
        if (opBit1 == 'q') {
            __asm {
                mov qword ptr[r10], r11
            }
        }
        else if (opBit1 == 'd') {
            if (opType1 == 'r') {
                __asm {
                    mov qword ptr[r10], r11
                }
            }
            else {
                __asm {
                    mov dword ptr[r10], r11d
                }
            }
        }
        else if (opBit1 == 'w') {
            __asm {
                mov word ptr[r10], r11w
            }
        }
        else if (opBit1 == 'b') {
            __asm {
                mov byte ptr[r10], r11b
            }
        }
    }
    else if (mnemonicIndex != nopIndex) {
        if (mnemonicIndex == pushIndex) {
            vtRegs[14] -= sizeof(DWORD_PTR);
            *(PDWORD_PTR)(vtRegs[14]) = *(PDWORD_PTR)(opAddr1);
        }
        else if (mnemonicIndex == popIndex) {
            *(PDWORD_PTR)(opAddr1) = *(PDWORD_PTR)(vtRegs[14]);
            vtRegs[14] += sizeof(DWORD_PTR);
        }
        else if (mnemonicIndex == callIndex) {
            DWORD_PTR realRSP;
            DWORD_PTR callAddr = *(PDWORD_PTR)opAddr1;
            // BOF 内部函数调用
            if (callAddr < 0xFFFF) {
                vtRegs[14] -= sizeof(DWORD_PTR);
                *(PDWORD_PTR)vtRegs[14] = vtRegs[16] + 5;
                vtRegs[16] = callAddr;
            }
            else {
                __asm {
                    // 保存真实栈顶
                    mov realRSP, rsp
                    // 虚拟寄存器 覆盖 真实寄存器
                    mov rax, qword ptr[vtRegs]
                    mov rbx, qword ptr[rax + 0x08]
                    mov rcx, qword ptr[rax + 0x10]
                    mov rdx, qword ptr[rax + 0x18]
                    mov rsi, qword ptr[rax + 0x20]
                    mov rdi, qword ptr[rax + 0x28]
                    mov r8, qword ptr[rax + 0x30]
                    mov r9, qword ptr[rax + 0x38]
                    mov r10, qword ptr[rax + 0x40]
                    mov r11, qword ptr[rax + 0x48]
                    mov r12, qword ptr[rax + 0x50]
                    mov r13, qword ptr[rax + 0x58]
                    mov r14, qword ptr[rax + 0x60]
                    mov r15, qword ptr[rax + 0x68]
                    mov rsp, qword ptr[rax + 0x70]
                    mov rax, qword ptr[rax]
                    // 调用 Windows API
                    call callAddr
                    // 真实寄存器 覆盖 虚拟寄存器
                    push rax
                    mov rax, qword ptr[vtRegs]
                    mov qword ptr[rax + 0x08], rbx
                    mov qword ptr[rax + 0x10], rcx
                    mov qword ptr[rax + 0x18], rdx
                    mov qword ptr[rax + 0x20], rsi
                    mov qword ptr[rax + 0x28], rdi
                    mov qword ptr[rax + 0x30], r8
                    mov qword ptr[rax + 0x38], r9
                    mov qword ptr[rax + 0x40], r10
                    mov qword ptr[rax + 0x48], r11
                    mov qword ptr[rax + 0x50], r12
                    mov qword ptr[rax + 0x58], r13
                    mov qword ptr[rax + 0x60], r14
                    mov qword ptr[rax + 0x68], r15
                    pop rcx
                    mov qword ptr[rax + 0x70], rsp
                    mov qword ptr[rax], rcx
                    // 还原真实栈顶
                    mov rsp, realRSP
                }
            }
        }
        else if (mnemonicIndex == retIndex) {
            vtRegs[16] = *(PDWORD_PTR)vtRegs[14];
            vtRegs[14] += sizeof(DWORD_PTR);
        }
        else if (mnemonicIndex == movzxIndex) {
            __asm {
                mov r10, qword ptr[opAddr1]
                mov r11, qword ptr[r10]
                mov r12, qword ptr[opAddr2]
                mov r12, qword ptr[r12]
            }
            if (opBit1 == 'q' || opBit1 == 'd') {
                if (opBit2 == 'w') {
                    __asm {
                        movzx r11, r12w
                    }
                }
                else {
                    __asm {
                        movzx r11, r12b
                    }
                }
            }
            else {
                __asm {
                    movzx r11w, r12b
                }
            }
            __asm {
                mov qword ptr[r10], r11
            }
        }
        else if (mnemonicIndex == movsxdIndex) {
            *(PDWORD64)opAddr1 = *(PDWORD)opAddr2;
        }
        else if (mnemonicIndex == cmpIndex) {
            __asm {
                mov r11, qword ptr[opAddr1]
                mov r11, qword ptr[r11]
                mov r12, qword ptr[opAddr2]
                mov r12, qword ptr[r12]
                // opBit1 == 'q'
                movsx eax, byte ptr[opBit1]
                cmp eax, 0x71
                jne checkDWORD
                cmp r11, r12
                jmp setEFL
                // opBit1 == 'd'
                checkDWORD:
                cmp eax, 0x64
                jne checkWORD
                cmp r11d, r12d
                jmp setEFL
                // opBit1 == 'w'
                checkWORD:
                cmp eax, 0x77
                jne checkBYTE
                cmp r11w, r12w
                jmp setEFL
                // opBit1 == 'b'
                checkBYTE:
                cmp r11b, r12b
                setEFL:
                pushf
                pop rax
                mov rcx, qword ptr[vtRegs]
                mov qword ptr[rcx + 0x88], rax // vtEFL
            }
        }
        else if (mnemonicIndex == testIndex) {
            __asm {
                mov r11, qword ptr[opAddr1]
                mov r11, qword ptr[r11]
                mov r12, qword ptr[opAddr2]
                mov r12, qword ptr[r12]
                // opBit1 == 'q'
                movsx eax, byte ptr[opBit1]
                cmp eax, 0x71
                jne checkDWORD
                test r11, r12
                jmp setEFL
                // opBit1 == 'd'
                checkDWORD:
                cmp eax, 0x64
                jne checkWORD
                test r11d, r12d
                jmp setEFL
                // opBit1 == 'w'
                checkWORD:
                cmp eax, 0x77
                jne checkBYTE
                test r11w, r12w
                jmp setEFL
                // opBit1 == 'b'
                checkBYTE:
                test r11b, r12b
                setEFL:
                pushf
                pop rax
                mov rcx, qword ptr[vtRegs]
                mov qword ptr[rcx + 0x88], rax // vtEFL
            }
        }
        else if (mnemonicIndex == shlIndex) {
            if (opBit1 == 'q') {
                *(PDWORD64)opAddr1 <<= *(PDWORD64)opAddr2;
            }
            else if (opBit1 == 'd') {
                if (opType1 == 'r') {
                    *(PDWORD_PTR)opAddr1 = *(PDWORD)opAddr1 << *(PDWORD)opAddr2;
                }
                else {
                    *(PDWORD)opAddr1 <<= *(PDWORD)opAddr2;
                }
            }
            else if (opBit1 == 'w') {
                *(PWORD)opAddr1 <<= *(PWORD)opAddr2;
            }
            else if (opBit1 == 'b') {
                *(PBYTE)opAddr1 <<= *(PBYTE)opAddr2;
            }
        }
        else if (mnemonicIndex == shrIndex) {
            if (opBit1 == 'q') {
                *(PDWORD64)opAddr1 >>= *(PDWORD64)opAddr2;
            }
            else if (opBit1 == 'd') {
                if (opType1 == 'r') {
                    *(PDWORD_PTR)opAddr1 = *(PDWORD)opAddr1 >> *(PDWORD)opAddr2;
                }
                else {
                    *(PDWORD)opAddr1 >>= *(PDWORD)opAddr2;
                }
            }
            else if (opBit1 == 'w') {
                *(PWORD)opAddr1 >>= *(PWORD)opAddr2;
            }
            else if (opBit1 == 'b') {
                *(PBYTE)opAddr1 >>= *(PBYTE)opAddr2;
            }
        }
    }

    if (vtRegs[16] != vtRIP) {
        return 1;
    }
    return 0;
}