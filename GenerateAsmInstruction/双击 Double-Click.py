import re
import binascii
from capstone import Cs, CS_ARCH_X86, CS_MODE_64

def ParseShellCode(shellcode):
    asmByte = binascii.unhexlify(shellcode)
    cs = Cs(CS_ARCH_X86, CS_MODE_64)
    instructions = cs.disasm(asmByte, 0)

    print('    AsmName PROC')
    for instruction in instructions:
        ops = re.sub(r'0x([0-9a-f]+)', r'0\1h', instruction.op_str)
        print('    ' + instruction.mnemonic + ' ' + ops)
    print('    AsmName ENDP')

if __name__ == '__main__':
    with open('ShellCode.txt', 'r') as file:
        shellcode = file.read().replace(' ', '').replace('\n', '')
    ParseShellCode(shellcode)
    print('\n跳转指令请结合 Debug 手动处理')
    input()