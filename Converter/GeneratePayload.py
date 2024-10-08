import re
import struct

registers64 = ['rax', 'rbx', 'rcx', 'rdx', 'rsi', 'rdi', 'r8', 'r9', 'r10', 'r11', 'r12', 'r13', 'r14', 'r15', 'rsp', 'rbp']
registers32 = ['eax', 'ebx', 'ecx', 'edx', 'esi', 'edi', 'r8d', 'r9d', 'r10d', 'r11d', 'r12d', 'r13d', 'r14d', 'r15d', 'esp', 'ebp']
registers16 = ['ax', 'bx', 'cx', 'dx', 'si', 'di', 'r8w', 'r9w', 'r10w', 'r11w', 'r12w', 'r13w', 'r14w', 'r15w', 'sp', 'bp']
registersLow8 = ['al', 'bl', 'cl', 'dl', 'sil', 'dil', 'r8b', 'r9b', 'r10b', 'r11b', 'r12b', 'r13b', 'r14b', 'r15b', 'spl', 'bpl']

mnemonics = ['push', 'pop', 'call', 'ret', 'movzx', 'movsxd', 'cmp', 'test', 'shl', 'shr', 'nop', 'mov', 'movabs', 'lea', 'add', 'inc', 'sub', 'dec', 'and', 'or', 'xor', 'jmp', 'je', 'jne', 'jbe', 'jl', 'jge', 'jle']

def FormatOp(op):
    # 不存在 / 立即数
    if op == '' or op[0] == 'i':
        return b'?', op

    # 寄存器 -> 相对偏移
    if '[' not in op:
        if op == 'rip':
            return b'q', 'r0000000000000000'
        if op in registers64:
            return b'q', 'q' + hex(registers64.index(op) * 8)[2:]
        if op in registers32:
            return b'd', 'd' + hex(registers32.index(op) * 8)[2:]
        if op in registers16:
            return b'w', 'w' + hex(registers16.index(op) * 8)[2:]
        if op in registersLow8:
            return b'b', 'b' + hex(registersLow8.index(op) * 8)[2:]

    opBit = b'?'
    if 'ptr' in op:
        if 'qword' in op:
            opBit = b'q'
        elif 'dword' in op:
            opBit = b'd'
        elif 'word' in op:
            opBit = b'w'
        elif 'byte' in op:
            opBit = b'b'

    # [寄存器] -> [相对偏移]
    words = op.replace('[', '').replace(']', '').split(' ')
    for word in words:
        if word == 'rip':
            op = op.replace(word, 'r0000000000000000')
        elif word in registers64:
            op = op.replace(word, 'q' + hex(registers64.index(word) * 8)[2:])
        elif word in registers32:
            op = op.replace(word, 'd' + hex(registers32.index(word) * 8)[2:])
        elif word in registers16:
            op = op.replace(word, 'w' + hex(registers16.index(word) * 8)[2:])
        elif word in registersLow8:
            op = op.replace(word, 'b' + hex(registersLow8.index(word) * 8)[2:])

    # [相对偏移] -> l相对偏移
    op = re.sub(r'^(\[.+\])', lambda match: 'l' + match.group(1).replace('[', '').replace(']', ''), op)
    # xxx ptr [相对偏移] -> p相对偏移
    op = re.sub(r'.+(\[.+\])', lambda match: 'p' + match.group(1).replace('[', '').replace(']', ''), op)
    return opBit, op.replace(' ', '')

def FormatAsm(asm):
    if '*' in asm:
        raise Exception('无法处理 [] 中的 *')
    asm = asm.replace('+', '++').replace('- ', '-- ')

    selfAsm = b''
    for instruction in asm.split('\n'):
        asmInfo = instruction.split('_')
        if len(asmInfo) != 3:
            if len(instruction) != 0:
                raise Exception('无法处理汇编指令: ' + instruction)
            else:
                continue

        address = struct.pack('<H', int(asmInfo[0].replace('0x', ''), 16))
        mnemonic = struct.pack('B', mnemonics.index(asmInfo[1]))

        # 确保有两个操作数, 0x立即数 -> i立即数
        ops = asmInfo[2].replace('0x', 'i').split(', ')
        if len(ops) == 1:
            ops += ['']
        if len(ops) != 2:
            raise Exception('只能处理 1 ~ 2 个操作数的汇编指令: ' + instruction)

        opBit1, op1 = FormatOp(ops[0])
        opBit2, op2 = FormatOp(ops[1])
        # 无法直接确定 op1 位数的情况: 立即数、不存在
        if opBit1 == b'?':
            opBit1 = b'q'
        # 无法直接确定 op2 位数的情况: lea、立即数、不存在
        if opBit2 == b'?':
            opBit2 = opBit1

        selfAsm += address + mnemonic + opBit1 + op1.encode() + b'\x00' + opBit2 + op2.encode() + b'\x00'
    return selfAsm

def GeneratePayload(asm, rdata, textRelocNames, bofFuncOffsetMap):
    selfAsm = FormatAsm(asm)

    print('请手动在 Instruction.cpp 的 InvokeInstruction 函数开头定义:\nconst int ', end='')
    for i in range(len(mnemonics)):
        print(f'{mnemonics[i]}Index = {i}', end='')
        if i < len(mnemonics)-1:
            print(', ', end='')
    print(f';\nif (mnemonicIndex < 0 || mnemonicIndex > {len(mnemonics)-1}) ' + '{\nreturn 2;\n}')

    bofFuncOffsetMap = struct.pack('<H', len(bofFuncOffsetMap)) + bofFuncOffsetMap
    textRelocNames = struct.pack('<H', len(textRelocNames)) + textRelocNames
    selfAsm = struct.pack('<H', len(selfAsm)) + selfAsm
    return bofFuncOffsetMap + textRelocNames + selfAsm + rdata