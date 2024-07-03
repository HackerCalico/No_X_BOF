#include <iostream>
#include <windows.h>

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
typedef size_t(WINAPI* pStrlen)(const char*);
typedef int(WINAPI* pSprintf_s)(char*, size_t, const char*, ...);
typedef HANDLE(WINAPI* pFindFirstFileA)(LPCSTR, LPWIN32_FIND_DATAA);
typedef BOOL(WINAPI* pFindNextFileA)(HANDLE, LPWIN32_FIND_DATAA);
typedef BOOL(WINAPI* pFindClose)(HANDLE);
typedef DWORD(WINAPI* pGetFullPathNameA)(LPCSTR, DWORD, LPSTR, LPSTR*);
typedef BOOL(WINAPI* pFileTimeToSystemTime)(FILETIME*, LPSYSTEMTIME);

#pragma code_seg(".shell")

// 获取文件信息列表
void GetFileInfoList(char* commandPara, int commandParaLength, char** pOutputData, int* pOutputDataLength, PVOID* pFuncAddr) {
    *pOutputData = (char*)((pMalloc)(pFuncAddr[0]))(330);
    **pOutputData = '0';
    *pOutputDataLength = 1;

    WIN32_FIND_DATAA findData;
    HANDLE hFind = ((pFindFirstFileA)pFuncAddr[11])(commandPara, &findData);
    if (hFind != INVALID_HANDLE_VALUE) {
        if (!((pGetFullPathNameA)pFuncAddr[14])(commandPara, MAX_PATH, *pOutputData, NULL)) {
            ((pFindClose)pFuncAddr[13])(hFind);
            return;
        }
        do {
            if (*findData.cFileName != '.') {
                SYSTEMTIME systemTime;
                if (!((pFileTimeToSystemTime)pFuncAddr[15])(&(findData.ftLastWriteTime), &systemTime)) {
                    ((pFindClose)pFuncAddr[13])(hFind);
                    break;
                }
                volatile char format[] = { '\n', '%', 'd', ',', '%', 's', ',', '%', 'l', 'l', 'u', ',', '%', '0', '4', 'd', '.', '%', '0', '2', 'd', '.', '%', '0', '2', 'd', ' ', '%', '0', '2', 'd', ':', '%', '0', '2', 'd', ':', '%', '0', '2', 'd', '\0' };
                ((pSprintf_s)pFuncAddr[6])(*pOutputData + ((pStrlen)pFuncAddr[3])(*pOutputData), 330, (char*)format, (findData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) ? 1 : 0, findData.cFileName, ((ULONGLONG)findData.nFileSizeHigh << 32) | findData.nFileSizeLow, systemTime.wYear, systemTime.wMonth, systemTime.wDay, systemTime.wHour, systemTime.wMinute, systemTime.wSecond);
                *pOutputData = (char*)((pRealloc)pFuncAddr[1])(*pOutputData, ((pStrlen)pFuncAddr[3])(*pOutputData) + 330);
            }
        } while (((pFindNextFileA)pFuncAddr[12])(hFind, &findData));
    }
    *pOutputDataLength = ((pStrlen)pFuncAddr[3])(*pOutputData);
}

#pragma code_seg(".text")

int main() {
    char commandPara[] = "C:\\Windows\\System32\\*";
    int commandParaLength = strlen(commandPara) + 1;
    char* outputData;
    int outputDataLength;
    PVOID funcAddr[] = { malloc, realloc, free, strlen, strtol, ((errno_t(*)(char*, rsize_t, const char*))strcpy_s), ((int(*)(char*, size_t, const char*, ...))sprintf_s), CloseHandle, CreateProcessA, CreatePipe, ReadFile, FindFirstFileA, FindNextFileA, FindClose, GetFullPathNameA, FileTimeToSystemTime };
    GetFileInfoList(commandPara, commandParaLength, &outputData, &outputDataLength, funcAddr);

    std::cout << outputData;
}
