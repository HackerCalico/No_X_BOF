#ifndef INSTRUCTION_H
#define INSTRUCTION_H

int InvokeInstruction(int mnemonicIndex, char opType1, char opBit1, DWORD_PTR opAddr1, char opBit2, DWORD_PTR opAddr2, DWORD_PTR vtRegs[], char obfMap[]);

#endif