#pragma once

EXTERN_C void AsmCall(DWORD_PTR opAddr1, PDWORD_PTR pVtRegs);
EXTERN_C void AsmMovsx(char opBit1, DWORD_PTR opAddr1, char opBit2, DWORD_PTR opAddr2);
EXTERN_C void AsmMovzx(char opBit1, DWORD_PTR opAddr1, char opBit2, DWORD_PTR opAddr2);
EXTERN_C void AsmCmp(char opBit1, DWORD_PTR opAddr1, DWORD_PTR opAddr2, PDWORD_PTR pVtRegs);
EXTERN_C void AsmTest(char opBit1, DWORD_PTR opAddr1, DWORD_PTR opAddr2, PDWORD_PTR pVtRegs);
EXTERN_C int AsmJe(DWORD_PTR vtEFL);
EXTERN_C int AsmJne(DWORD_PTR vtEFL);
EXTERN_C int AsmJbe(DWORD_PTR vtEFL);
EXTERN_C int AsmJl(DWORD_PTR vtEFL);
EXTERN_C int AsmJge(DWORD_PTR vtEFL);
EXTERN_C int AsmJle(DWORD_PTR vtEFL);