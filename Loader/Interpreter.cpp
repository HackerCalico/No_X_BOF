#include "Interpreter.h"
#include "Instruction.h"

PVOID pVtStack = NULL;
DWORD_PTR initRIP = 0x12345678;
map<string, string> winApiRelocAddrMap;

// 计算 (未考虑“*”)
DWORD_PTR Calculate(DWORD_PTR number1, DWORD_PTR number2, char symbol, char obfMap[]) {
    if (symbol == obfMap['+']) {
        return number1 + number2;
    }
    else if (symbol == obfMap['-']) {
        return number1 - number2;
    }
    throw exception("Symbol incorrect.");
}

// 解析算式 (未考虑“*”)
void ParseFormula(char* op, char* formula[], char* symbols[], char obfMap[]) {
    int index = 1;
    int opLength = strlen(op);
    for (int i = 0; i < opLength; i++) {
        if (*(op + i) == obfMap['+']) {
            formula[index] = symbols[0];
            *(op + i) = '\0';
            index += 2;
        }
        else if (*(op + i) == obfMap['-']) {
            formula[index] = symbols[1];
            *(op + i) = '\0';
            index += 2;
        }
    }
    index = 0;
    for (int i = 0; i < opLength; i += strlen(op + i) + 1) {
        formula[index] = op + i;
        index += 2;
    }
}

// 获取操作数值的 Type(i立即数/r寄存器/m内存空间) & 地址
DWORD_PTR GetOpTypeAndAddr(char* op, char* pOpType1, DWORD_PTR vtRegs[], DWORD_PTR* pOpNumber, char obfMap[]) {
    // cout << "CurrentOp: " << op << endl; // 调试
    // 立即数
    if (*op == obfMap['i']) {
        if (pOpNumber == NULL) {
            throw exception("IMM failed.");
        }
        char* endPtr;
        *pOpNumber = strtoull(op + 1, &endPtr, 16);
        if (*endPtr != '\0') {
            throw exception("IMM incorrect.");
        }
        if (pOpType1 != NULL) {
            *pOpType1 = 'i';
        }
        return (DWORD_PTR)pOpNumber;
    }
    // lea [] / ptr []
    else if (*op == obfMap['l'] || *op == obfMap['p']) {
        // 解析算式 (未考虑“*”)
        char tempOp[50] = "";
        string add = string(1, obfMap['+']);
        string sub = string(1, obfMap['-']);
        char* symbols[] = { (char*)add.c_str(), (char*)sub.c_str() };
        char* formula[] = { NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL };
        strcpy_s(tempOp, sizeof(tempOp), op + 1);
        ParseFormula(tempOp, formula, symbols, obfMap);
        // 计算 (未考虑“*”)
        char symbol = obfMap['+'];
        DWORD_PTR number1 = 0;
        DWORD_PTR number2 = NULL;
        for (int i = 0; formula[i] != NULL; i++) {
            if (*formula[i] == obfMap['i']) {
                DWORD_PTR tempNumber; // 存储数字
                GetOpTypeAndAddr(formula[i], NULL, vtRegs, &tempNumber, obfMap);
                number1 = Calculate(number1, tempNumber, symbol, obfMap);
            }
            else if (*formula[i] == obfMap['q']) {
                number2 = *(PDWORD64)GetOpTypeAndAddr(formula[i], NULL, vtRegs, NULL, obfMap);
                number1 = Calculate(number1, number2, symbol, obfMap);
            }
            else if (*formula[i] == obfMap['d']) {
                number2 = *(PDWORD)GetOpTypeAndAddr(formula[i], NULL, vtRegs, NULL, obfMap);
                number1 = Calculate(number1, number2, symbol, obfMap);
            }
            else if (*formula[i] == obfMap['w']) {
                number2 = *(PWORD)GetOpTypeAndAddr(formula[i], NULL, vtRegs, NULL, obfMap);
                number1 = Calculate(number1, number2, symbol, obfMap);
            }
            else if (*formula[i] == obfMap['b']) {
                number2 = *(PBYTE)GetOpTypeAndAddr(formula[i], NULL, vtRegs, NULL, obfMap);
                number1 = Calculate(number1, number2, symbol, obfMap);
            }
            else if (*formula[i] == obfMap['+']) {
                symbol = obfMap['+'];
            }
            else if (*formula[i] == obfMap['-']) {
                symbol = obfMap['-'];
            }
        }
        // lea []
        if (*op == obfMap['l']) {
            if (pOpNumber == NULL) {
                throw exception("lea failed.");
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
    else if (*op == obfMap['q'] || *op == obfMap['d'] || *op == obfMap['w'] || *op == obfMap['b']) {
        char* endPtr;
        DWORD_PTR offset = strtoull(op + 1, &endPtr, 16);
        if (*endPtr != '\0') {
            throw exception("Reg offset incorrect.");
        }
        if (pOpType1 != NULL) {
            *pOpType1 = 'r';
        }
        return (DWORD_PTR)vtRegs + offset;
    }
    throw exception("Op type not exist."); // 不是 OpType
}

// 运行 BOF 函数
void RunBofFunc(char* bofFuncName, BofPayload& bofPayload, DWORD_PTR vtRegs[]) {
    if (bofPayload.bofFuncOffsetMap.find(bofFuncName) == bofPayload.bofFuncOffsetMap.end()) {
        throw exception("BofFuncName does not exist");
    }
    DWORD_PTR bofFuncVtAddr = bofPayload.bofFuncOffsetMap[bofFuncName];
    for (int index = bofPayload.indexMap[bofFuncVtAddr]; index < bofPayload.selfAsmLength; index++) {
        vtRegs[16] = bofPayload.vtAddrMap[index];
        int mnemonicIndex = bofPayload.selfAsmMap[vtRegs[16]]->mnemonicIndex;
        char* opBit1 = bofPayload.selfAsmMap[vtRegs[16]]->opBit1;
        char* op1 = bofPayload.selfAsmMap[vtRegs[16]]->op1;
        char* opBit2 = bofPayload.selfAsmMap[vtRegs[16]]->opBit2;
        char* op2 = bofPayload.selfAsmMap[vtRegs[16]]->op2;

        // 调试
        /*cout << "Register:\n";
        for (int i = 0; i < 18; i++) {
            cout << dec << i << " " << hex << vtRegs[i] << endl;
        }
        cout << "Index:" << dec << index;
        cout << " VtAddr:" << hex << vtRegs[16];
        cout << " MnemonicIndex:" << dec << mnemonicIndex;
        cout << " OpBit1:" << opBit1;
        cout << " Op1:" << op1;
        cout << " OpBit2:" << opBit2;
        cout << " Op2:" << op2 << endl;*/

        // 获取两个操作数的 Type(i立即数/r寄存器/m内存空间) & 地址
        char opType1 = NULL;
        DWORD_PTR opAddr1 = NULL;
        DWORD_PTR opAddr2 = NULL;
        DWORD_PTR opNumber = NULL; // 存储数字
        if (*op1 != '\0') {
            opAddr1 = GetOpTypeAndAddr(op1, &opType1, vtRegs, &opNumber, bofPayload.obfMap);
            if (opAddr1 == NULL || opType1 == NULL) {
                throw exception("Op1 incorrect.");
            }
            // cout << "Op1 Addr: " << hex << opAddr1 << endl; // 调试
        }
        else if (*op2 != '\0') {
            throw exception("Op2 incorrect.");
        }
        if (*op2 != '\0') {
            opAddr2 = GetOpTypeAndAddr(op2, NULL, vtRegs, &opNumber, bofPayload.obfMap);
            if (opAddr2 == NULL) {
                throw exception("Op2 incorrect.");
            }
            // cout << "Op2 Addr: " << hex << opAddr2 << endl; // 调试
        }

        // 调用指令
        if (InvokeInstruction(mnemonicIndex, opType1, *opBit1, opAddr1, *opBit2, opAddr2, vtRegs, bofPayload.obfMap)) {
            if (vtRegs[16] == initRIP) {
                return;
            }
            index = bofPayload.indexMap[vtRegs[16]]; // Jcc 指令跳转
            index--;
        }
    }
}

void MagicInvoke(char* bofFuncName, char* commandPara, int commandParaLength, char*& outputData, int& outputDataLength, PVOID specialParaList[], BofPayload& bofPayload) {
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
    vtRegs[16] = initRIP;
    vtRegs[14] = vtRegs[15] = (DWORD_PTR)pVtStack + 0x9000;

    // 设置虚拟寄存器的初值
    /*
    * BofFunc(commandPara, commandParaLength, &outputData, &outputDataLength, specialParaList);
    * lea rax, [specialParaList]
    * mov qword ptr [rsp+20h], rax
    * lea r9, [outputDataLength]
    * lea r8, [outputData]
    * mov edx, dword ptr [commandParaLength]
    * lea rcx, [commandPara]
    * call BofFunc
    */
    vtRegs[0] = (DWORD_PTR)specialParaList;
    *(PDWORD_PTR)(vtRegs[14] + 0x20) = vtRegs[0];
    vtRegs[7] = (DWORD_PTR)&outputDataLength;
    vtRegs[6] = (DWORD_PTR)&outputData;
    vtRegs[3] = commandParaLength;
    vtRegs[2] = (DWORD_PTR)commandPara;
    vtRegs[14] -= sizeof(DWORD_PTR);
    *(PDWORD_PTR)vtRegs[14] = vtRegs[16];

    // 运行 BOF 函数
    RunBofFunc(bofFuncName, bofPayload, vtRegs);
}

// .text 重定位
char* TextReloc(char* op, int& relocIndex, vector<string>& textRelocNameVector, BofPayload& bofPayload) {
    char relocAddr[18];
    char* rip = strstr(op, (string(1, bofPayload.obfMap['r']) + "0000000000000000").c_str());
    if (rip != NULL) {
        if (!strcmp(textRelocNameVector[relocIndex].c_str(), ".rdata")) {
            sprintf_s(relocAddr, sizeof(relocAddr), "%c%016llx", bofPayload.obfMap['i'], (DWORD_PTR)bofPayload.pRdata);
            memcpy(rip, relocAddr, 17);
        }
        else {
            if (winApiRelocAddrMap.find(textRelocNameVector[relocIndex].c_str()) != winApiRelocAddrMap.end()) {
                memcpy(rip, winApiRelocAddrMap[textRelocNameVector[relocIndex].c_str()].c_str(), 17);
            }
            else {
                char dllName[50];
                sprintf_s(dllName, sizeof(dllName), "%s", textRelocNameVector[relocIndex].c_str());
                char* dollar = strchr(dllName, '$');
                if (dollar != NULL) {
                    *dollar = '\0';
                    char* funcName = dllName + strlen(dllName) + 1;
                    HMODULE hDll;
                    if (!strcmp(dllName, "Ntdll") || !strcmp(dllName, "Kernel32")) {
                        hDll = GetModuleHandleA(dllName);
                    }
                    else {
                        hDll = LoadLibraryA(dllName);
                    }
                    if (hDll != NULL) {
                        DWORD_PTR funcAddr = (DWORD_PTR)GetProcAddress(hDll, funcName);
                        if (funcAddr != NULL) {
                            PVOID winApiPtr = malloc(sizeof(DWORD_PTR));
                            *(PDWORD_PTR)winApiPtr = funcAddr;
                            sprintf_s(relocAddr, sizeof(relocAddr), "%c%016llx", bofPayload.obfMap['i'], (DWORD_PTR)winApiPtr);
                            winApiRelocAddrMap[textRelocNameVector[relocIndex].c_str()] = relocAddr;
                            memcpy(rip, relocAddr, 17);
                        }
                        else {
                            throw exception("GetProcAddress failed.");
                        }
                    }
                    else {
                        throw exception("Get Dll failed.");
                    }
                }
                else {
                    throw exception("Can not reloc.");
                }
            }
        }
        relocIndex++;
    }
    return op;
}

// 解析 Payload
void ParsePayload(char* payload, int payloadLength, BofPayload& bofPayload) {
    // 提取 BOF 函数偏移 + .text 重定位名称 + 混淆映射 + 自定义汇编 + .rdata
    int bofFuncOffsetDictLength = strlen(payload);
    bofPayload.bofFuncOffsetMap = (json::parse(payload)).get<map<string, int>>();
    // cout << "BofFuncOffsetMap:\n" << payload << endl; // 调试

    char* textRelocNameList = payload + bofFuncOffsetDictLength + 1;
    int textRelocNameListLength = strlen(textRelocNameList);
    vector<string> textRelocNameVector = json::parse(textRelocNameList);
    // cout << "TextRelocNameVector:\n" << textRelocNameList << endl; // 调试

    char* obfDict = textRelocNameList + textRelocNameListLength + 1;
    int obfDictLength = strlen(obfDict);
    map<string, string> obfDictMap = (json::parse(obfDict)).get<map<string, string>>();
    bofPayload.obfMap = (char*)malloc(sizeof(char) * 130);
    for (const auto& pair : obfDictMap) {
        bofPayload.obfMap[*pair.first.c_str()] = pair.second[0];
    }
    // cout << "ObfDict:\n" << obfDict << endl; // 调试

    char* selfAsmOriginal = obfDict + obfDictLength + 1;
    int selfAsmSize = strlen(selfAsmOriginal);
    bofPayload.selfAsm = (char*)malloc(selfAsmSize + 1);
    memcpy(bofPayload.selfAsm, selfAsmOriginal, selfAsmSize + 1);
    // cout << "SelfAsm:\n" << bofPayload.selfAsm << endl; // 调试

    bofPayload.rdataLength = payloadLength - bofFuncOffsetDictLength - textRelocNameListLength - obfDictLength - selfAsmSize - 4;
    bofPayload.pRdata = malloc(bofPayload.rdataLength);
    memcpy(bofPayload.pRdata, selfAsmOriginal + selfAsmSize + 1, bofPayload.rdataLength);
    // cout << "pRdata:\n" << bofPayload.pRdata << endl; // 调试

    // 解析自定义汇编 & .text 重定位
    for (int i = 0; i < selfAsmSize; i++) {
        if (*(bofPayload.selfAsm + i) == bofPayload.obfMap['_']) {
            *(bofPayload.selfAsm + i) = '\0';
        }
    }
    int index = 0;
    int offset = 0;
    int relocIndex = 0;
    while (*(bofPayload.selfAsm + offset) != '\0') {
        // 虚拟地址
        char* endPtr;
        DWORD_PTR vtAddr = strtoull(bofPayload.selfAsm + offset, &endPtr, 16);
        if (*endPtr != '\0') {
            throw exception("VtAddr incorrect.");
        }
        offset += strlen(bofPayload.selfAsm + offset) + 1;
        bofPayload.vtAddrMap[index] = vtAddr;
        bofPayload.indexMap[vtAddr] = index;

        SelfAsm* pCurrentSelfAsm = (SelfAsm*)malloc(sizeof(SelfAsm));

        // 助记符序号
        pCurrentSelfAsm->mnemonicIndex = atoi(bofPayload.selfAsm + offset);
        offset += strlen(bofPayload.selfAsm + offset) + 1;

        // 操作数1 位数
        pCurrentSelfAsm->opBit1 = bofPayload.selfAsm + offset;
        offset += strlen(bofPayload.selfAsm + offset) + 1;

        // 操作数1
        pCurrentSelfAsm->op1 = TextReloc(bofPayload.selfAsm + offset, relocIndex, textRelocNameVector, bofPayload);
        offset += strlen(bofPayload.selfAsm + offset) + 1;

        // 操作数2 位数
        pCurrentSelfAsm->opBit2 = bofPayload.selfAsm + offset;
        offset += strlen(bofPayload.selfAsm + offset) + 1;

        // 操作数2
        pCurrentSelfAsm->op2 = TextReloc(bofPayload.selfAsm + offset, relocIndex, textRelocNameVector, bofPayload);
        offset += strlen(bofPayload.selfAsm + offset) + 1;

        bofPayload.selfAsmMap[vtAddr] = pCurrentSelfAsm;
        index++;
    }
    bofPayload.selfAsmLength = index;
    textRelocNameVector.clear();

    // 调试
    /*cout << "SelfAsmLength: " << bofPayload.selfAsmLength << endl;
    for (int index = 0; index < bofPayload.selfAsmLength; index++) {
        cout << "Index: " << bofPayload.indexMap[bofPayload.vtAddrMap[index]];
        cout << " VtAddr: " << hex << bofPayload.vtAddrMap[index];
        SelfAsm* pSelfAsm = bofPayload.selfAsmMap[bofPayload.vtAddrMap[index]];
        cout << " MnemonicIndex: " << dec << pSelfAsm->mnemonicIndex;
        cout << " OpBit1: " << pSelfAsm->opBit1;
        cout << " Op1: " << pSelfAsm->op1;
        cout << " OpBit2: " << pSelfAsm->opBit2;
        cout << " Op2: " << pSelfAsm->op2 << endl;
    }*/
}