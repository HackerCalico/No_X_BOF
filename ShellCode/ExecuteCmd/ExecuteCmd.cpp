#include <iostream>
#include <windows.h>

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

typedef void* (WINAPI* pMalloc)(size_t);
typedef void* (WINAPI* pRealloc)(void*, size_t);
typedef BOOL(WINAPI* pCloseHandle)(HANDLE);
typedef BOOL(WINAPI* pCreateProcessA)(LPCSTR, LPSTR, LPSECURITY_ATTRIBUTES, LPSECURITY_ATTRIBUTES, BOOL, DWORD, LPVOID, LPCSTR, LPSTARTUPINFOA, LPPROCESS_INFORMATION);
typedef BOOL(WINAPI* pCreatePipe)(PHANDLE, PHANDLE, LPSECURITY_ATTRIBUTES, DWORD);
typedef BOOL(WINAPI* pReadFile)(HANDLE, LPVOID, DWORD, LPDWORD, LPOVERLAPPED);

#pragma code_seg(".shell")

// 执行 CMD 命令
void ExecuteCmd(char* commandPara, int commandParaLength, char** pOutputData, int* pOutputDataLength, PVOID* pFuncAddr) {
    *pOutputData = (char*)((pMalloc)(pFuncAddr[0]))(100);
    **pOutputData = '0';
    *pOutputDataLength = 1;

    HANDLE hRead, hWrite;
    SECURITY_ATTRIBUTES sa;
    sa.nLength = sizeof(SECURITY_ATTRIBUTES);
    sa.bInheritHandle = TRUE;
    sa.lpSecurityDescriptor = NULL;
    if (!((pCreatePipe)(pFuncAddr[9]))(&hRead, &hWrite, &sa, 0)) {
        return;
    }

    STARTUPINFOA si = { 0 };
    PROCESS_INFORMATION pi;
    si.cb = sizeof(STARTUPINFOA);
    si.hStdError = hWrite;
    si.hStdOutput = hWrite;
    si.wShowWindow = SW_HIDE;
    si.dwFlags = STARTF_USESHOWWINDOW | STARTF_USESTDHANDLES;
    if (!((pCreateProcessA)(pFuncAddr[8]))(NULL, commandPara, NULL, NULL, TRUE, 0, NULL, NULL, &si, &pi)) {
        ((pCloseHandle)(pFuncAddr[7]))(hRead);
        ((pCloseHandle)(pFuncAddr[7]))(hWrite);
        return;
    }
    ((pCloseHandle)(pFuncAddr[7]))(hWrite);
    ((pCloseHandle)(pFuncAddr[7]))(pi.hThread);
    ((pCloseHandle)(pFuncAddr[7]))(pi.hProcess);

    *pOutputDataLength = 0;
    DWORD currentReadLength;
    while (((pReadFile)(pFuncAddr[10]))(hRead, *pOutputData + *pOutputDataLength, 100, &currentReadLength, NULL) && currentReadLength != 0) {
        *pOutputDataLength += currentReadLength;
        *pOutputData = (char*)((pRealloc)(pFuncAddr[1]))(*pOutputData, *pOutputDataLength + 100);
    }
    ((pCloseHandle)(pFuncAddr[7]))(hRead);
}

#pragma code_seg(".text")

int main() {
    char commandPara[] = "cmd /c tasklist";
    int commandParaLength = strlen(commandPara) + 1;
    char* outputData;
    int outputDataLength;
    PVOID funcAddr[] = { malloc, realloc, free, strlen, strtol, ((errno_t(*)(char*, rsize_t, const char*))strcpy_s), ((int(*)(char*, size_t, const char*, ...))sprintf_s), CloseHandle, CreateProcessA, CreatePipe, ReadFile, FindFirstFileA, FindNextFileA, FindClose, GetFullPathNameA, FileTimeToSystemTime };
    ExecuteCmd(commandPara, commandParaLength, &outputData, &outputDataLength, funcAddr);

    *(outputData + outputDataLength) = '\0';
    cout << outputData;
}
