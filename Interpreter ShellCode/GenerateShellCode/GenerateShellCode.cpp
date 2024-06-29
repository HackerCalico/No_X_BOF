#include <iostream>
#include <windows.h>

using namespace std;

/*
* 因为机器码存在跨 Section 情况，所以不像一般情况直接提取出来就能运行
* 需要先映射到正确位置，并在开头添加 jmp MagicInvoke
* .text 舍弃
* _asmIns
* .magic
* .rdata
*/

int main(int argc, char* argv[]) {
    remove("Interpreter.bin");

    HANDLE hFile = CreateFileA(argv[1], GENERIC_READ, NULL, NULL, OPEN_EXISTING, 0, NULL);
    int fileSize = GetFileSize(hFile, NULL);
    DWORD_PTR fileAddr = (DWORD_PTR)malloc(fileSize);
    DWORD readSize;
    ReadFile(hFile, (PVOID)fileAddr, fileSize, &readSize, NULL);

    PIMAGE_DOS_HEADER pDos = (PIMAGE_DOS_HEADER)fileAddr;
    PIMAGE_NT_HEADERS pNt = (PIMAGE_NT_HEADERS)(fileAddr + pDos->e_lfanew);
    WORD numberOfSections = pNt->FileHeader.NumberOfSections;
    DWORD sizeOfImage = pNt->OptionalHeader.SizeOfImage;
    PIMAGE_SECTION_HEADER pSectionDir = (PIMAGE_SECTION_HEADER)((DWORD_PTR) & (pNt->OptionalHeader) + pNt->FileHeader.SizeOfOptionalHeader);

    DWORD_PTR _asmInsSectionAddr;
    DWORD_PTR magicSectionAddr;
    DWORD_PTR rdataSectionTailAddr;
    DWORD_PTR imageBase = (DWORD_PTR)malloc(sizeOfImage);
    for (int i = 0; i < numberOfSections; i++) {
        if (!strcmp((char*)pSectionDir[i].Name, "_asmIns")) {
            _asmInsSectionAddr = imageBase + pSectionDir[i].VirtualAddress;
        }
        else if (!strcmp((char*)pSectionDir[i].Name, ".magic")) {
            magicSectionAddr = imageBase + pSectionDir[i].VirtualAddress;

        }
        else if (!strcmp((char*)pSectionDir[i].Name, ".rdata")) {
            rdataSectionTailAddr = imageBase + pSectionDir[i].VirtualAddress + pSectionDir[i].SizeOfRawData;
            memset((PVOID)(fileAddr + pSectionDir[i].PointerToRawData), 0, pSectionDir[i].SizeOfRawData);
        }
        memcpy((PVOID)(imageBase + pSectionDir[i].VirtualAddress), (PVOID)(fileAddr + pSectionDir[i].PointerToRawData), pSectionDir[i].SizeOfRawData);
    }

    int shellcodeLength = 1 + sizeof(PDWORD_PTR) + rdataSectionTailAddr - _asmInsSectionAddr;
    PBYTE shellcode = (PBYTE)VirtualAlloc(NULL, shellcodeLength, MEM_COMMIT, PAGE_EXECUTE_READWRITE);
    DWORD_PTR magicInvokeAddr = magicSectionAddr - _asmInsSectionAddr + 4;
    *shellcode = '\xE9';
    memcpy(shellcode + 1, &magicInvokeAddr, sizeof(DWORD_PTR));
    memcpy(shellcode + 1 + sizeof(PDWORD_PTR), (PVOID)_asmInsSectionAddr, rdataSectionTailAddr - _asmInsSectionAddr);

    FILE* file;
    fopen_s(&file, "Interpreter.bin", "wb");
    fwrite(shellcode, 1, shellcodeLength, file);
}