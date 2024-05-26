#include <iostream>
#include <windows.h>

using namespace std;

/*
* 1.C/C++
* 常规: SDL检查(否)
* 优化: 优化(已禁用)
* 代码生成: 运行库(多线程)、安全检查(禁用安全检查)
* 2.链接器
* 清单文件: 生成清单(否)
* 调试: 生成调试信息(否)
*/

typedef void* (WINAPI* pMalloc)(size_t);
typedef void* (WINAPI* pRealloc)(void*, size_t);
typedef BOOL(WINAPI* pCreatePipe)(PHANDLE, PHANDLE, LPSECURITY_ATTRIBUTES, DWORD);
typedef BOOL(WINAPI* pCreateProcessA)(LPCSTR, LPSTR, LPSECURITY_ATTRIBUTES, LPSECURITY_ATTRIBUTES, BOOL, DWORD, LPVOID, LPCSTR, LPSTARTUPINFOA, LPPROCESS_INFORMATION);
typedef BOOL(WINAPI* pCloseHandle)(HANDLE);
typedef BOOL(WINAPI* pReadFile)(HANDLE, LPVOID, DWORD, LPDWORD, LPOVERLAPPED);

#pragma code_seg(".shell")

// 执行 CMD 命令
void ExecuteCmd(char* commandPara, char** pCommandOutput, int* pCommandOutputLength, PVOID* pFuncAddr) {
    HANDLE hRead, hWrite;
    SECURITY_ATTRIBUTES sa;
    sa.nLength = sizeof(SECURITY_ATTRIBUTES);
    sa.bInheritHandle = TRUE;
    sa.lpSecurityDescriptor = NULL;
    if (!((pCreatePipe)(pFuncAddr[2]))(&hRead, &hWrite, &sa, 0)) {
        return;
    }

    STARTUPINFOA si = { 0 };
    PROCESS_INFORMATION pi;
    si.cb = sizeof(STARTUPINFOA);
    si.hStdError = hWrite;
    si.hStdOutput = hWrite;
    si.wShowWindow = SW_HIDE;
    si.dwFlags = STARTF_USESHOWWINDOW | STARTF_USESTDHANDLES;
    if (!((pCreateProcessA)(pFuncAddr[3]))(NULL, commandPara, NULL, NULL, TRUE, 0, NULL, NULL, &si, &pi)) {
        ((pCloseHandle)(pFuncAddr[4]))(hRead);
        ((pCloseHandle)(pFuncAddr[4]))(hWrite);
        return;
    }
    ((pCloseHandle)(pFuncAddr[4]))(hWrite);
    ((pCloseHandle)(pFuncAddr[4]))(pi.hThread);
    ((pCloseHandle)(pFuncAddr[4]))(pi.hProcess);

    *pCommandOutputLength = 0;
    DWORD currentReadLength;
    *pCommandOutput = (char*)((pMalloc)(pFuncAddr[0]))(250);
    while (((pReadFile)(pFuncAddr[5]))(hRead, *pCommandOutput + *pCommandOutputLength, 100, &currentReadLength, NULL) && currentReadLength != 0) {
        *pCommandOutputLength += currentReadLength;
        if (*pCommandOutputLength > 100) {
            *pCommandOutput = (char*)((pRealloc)(pFuncAddr[1]))(*pCommandOutput, *pCommandOutputLength + 100);
        }
    }
    ((pCloseHandle)(pFuncAddr[4]))(hRead);
}

#pragma code_seg(".text")

int main() {
    char commandPara[] = "cmd /c tasklist";
    char* commandOutput;
    int commandOutputLength;
    PVOID funcAddr[] = { malloc, realloc, CreatePipe, CreateProcessA, CloseHandle, ReadFile };
    ExecuteCmd(commandPara, &commandOutput, &commandOutputLength, funcAddr);

    *(commandOutput + commandOutputLength) = '\0';
    cout << commandOutput;
}