#include <string>
#include <fstream>
#include <windows.h>
#include <unordered_map>

using namespace std;

// 虚拟寄存器
DWORD vtEAX;
DWORD vtEBX;
DWORD vtECX;
DWORD vtEDX;
DWORD vtESP;
DWORD vtEBP;
DWORD vtESI;
DWORD vtEDI;
DWORD vtEIP;

// 虚拟栈
PVOID pVtStack = VirtualAlloc(NULL, 0x10000, MEM_COMMIT, PAGE_READWRITE);

// 真实栈顶栈底
DWORD realESP;
DWORD realEBP;

// ———————————————————————————— 不重要的函数 ————————————————————————————

// 读取 ShellCode
vector<string> ReadShellCode(const char* filePath) {
    vector<string> asmCodes;
    ifstream file(filePath);
    string asmCode;
    while (getline(file, asmCode)) {
        asmCodes.push_back(asmCode);
    }
    file.close();
    return asmCodes;
}

// 提取操作符
string GetMnemonic(string asmCode) {
    return asmCode.substr(0, asmCode.find(' '));
}

// 提取操作数
string GetOperands(string asmCode) {
    return asmCode.substr(asmCode.find(' ') + 1);
}

// ———————————————————————————— 模拟运行指令 ————————————————————————————

DWORD GetDwordValue(string var) {
    if (var == "eax") {
        return vtEAX;
    }
    if (var == "ebx") {
        return vtEBX;
    }
    if (var == "ecx") {
        return vtECX;
    }
    if (var == "edx") {
        return vtEDX;
    }
    if (var == "esp") {
        return vtESP;
    }
    if (var == "ebp") {
        return vtEBP;
    }
    if (var == "esi") {
        return vtESI;
    }
    if (var == "edi") {
        return vtEDI;
    }
    return stoi(var, nullptr, 16);
}

void Push(DWORD value) {
    vtESP -= 4;
    *(PDWORD)vtESP = value;
}

void Mov(string operands) {
    if (operands == "ebp, esp") {
        vtEBP = vtESP;
    }
    else if (operands == "byte ptr [ebp - 1], 0") {
        *(PBYTE)(vtEBP - 1) = 0;
    }
    else if (operands == "esp, ebp") {
        vtESP = vtEBP;
    }
}

void Lea() {
    vtEAX = vtEBP - 1;
}

void Call() {
    // 记录原栈位置
    __asm {
        mov realESP, esp
        mov realEBP, ebp
    }

    // 进入虚拟栈
    __asm {
        mov esp, vtESP
        mov ebp, vtEBP
    }

    // 调用 Windows API
    __asm {
        call dword ptr [ebp + 8]
    }

    // 回到原栈位置
    __asm {
        mov esp, realESP
        mov ebp, realEBP
    }
}

void Pop() {
    vtEBP = *(PDWORD)vtESP;
    vtESP += 4;
}

void Ret() {
    vtEIP = *(PDWORD)vtESP;
    vtESP += 4;
}

// ———————————————————————————— 逐条解析指令 ————————————————————————————

// 解析指令
void Parse() {
    // 从文件读取指令到数组
    vector<string> asmCodes = ReadShellCode("ShellCode.txt");

    // 遍历指令进行解析
    for (vtEIP = 0; vtEIP < asmCodes.size(); vtEIP++) {
        // 提取操作符和操作数
        string mnemonic = GetMnemonic(asmCodes[vtEIP]);
        string operands = GetOperands(asmCodes[vtEIP]);

        // 模拟运行指令
        if (mnemonic == "push") {
            Push(GetDwordValue(operands));
        }
        else if (mnemonic == "mov") {
            Mov(operands);
        }
        else if (mnemonic == "lea") {
            Lea();
        }
        else if (mnemonic == "call") {
            Call();
        }
        else if (mnemonic == "pop") {
            Pop();
        }
        else if (mnemonic == "ret") {
            Ret();
        }
    }
}

// ———————————————————————————— 构造虚拟环境 ————————————————————————————

// 解释器
__declspec(naked) void Interpreter(...) {
    // 模拟初始寄存器
    DWORD currentESP;
    __asm {
        mov vtEAX, eax
        mov vtEBX, ebx
        mov vtECX, ecx
        mov vtEDX, edx
        mov currentESP, esp
        mov vtESI, esi
        mov vtEDI, edi
    }

    // 模拟初始栈
    vtEBP = vtESP = (DWORD)pVtStack + 0x9000;
    for (int i = currentESP + 4; i >= currentESP; i -= 4) {
        vtESP -= 4;
        *(PDWORD)vtESP = *(PDWORD)i;
    }

    // 解析指令
    __asm {
        call Parse
        ret
    }
}

// ———————————————————————————— 程序入口点 ————————————————————————————

int main() {
    // 通过解释器运行 ShellCode
    Interpreter((PDWORD)MessageBoxA);
}