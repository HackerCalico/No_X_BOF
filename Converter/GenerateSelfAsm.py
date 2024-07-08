import re

registers64 = ['rax', 'rbx', 'rcx', 'rdx', 'rsi', 'rdi', 'r8', 'r9', 'r10', 'r11', 'r12', 'r13', 'r14', 'r15', 'rsp', 'rbp']
registers32 = ['eax', 'ebx', 'ecx', 'edx', 'esi', 'edi', 'r8d', 'r9d', 'r10d', 'r11d', 'r12d', 'r13d', 'r14d', 'r15d', 'esp', 'ebp']
registers16 = ['ax', 'bx', 'cx', 'dx', 'si', 'di', 'r8w', 'r9w', 'r10w', 'r11w', 'r12w', 'r13w', 'r14w', 'r15w', 'sp', 'bp']
registersLow8 = ['al', 'bl', 'cl', 'dl', 'sil', 'dil', 'r8b', 'r9b', 'r10b', 'r11b', 'r12b', 'r13b', 'r14b', 'r15b', 'spl', 'bpl']

mnemonics = ['push', 'pop', 'call', 'ret', 'mov', 'movsx', 'movzx', 'movsxd', 'lea', 'add', 'inc', 'sub', 'cmp', 'imul', 'and', 'or', 'xor', 'test', 'shl', 'cdqe', 'repStosb', 'jmp', 'je', 'jne', 'jbe', 'jl', 'jle']

def FormatOp(op):
    # 不存在 or 立即数
    if op == '' or op[0] == 'i':
        return '?', op

    opBit = '?'
    if 'ptr' in op:
        if 'qword' in op:
            opBit = 'q'
        elif 'dword' in op:
            opBit = 'd'
        elif 'word' in op:
            opBit = 'w'
        elif 'byte' in op:
            opBit = 'b'

    # 寄存器 转为相对 虚拟寄存器数组 基址的偏移
    words = op.replace('[', '').replace(']', '').split(' ')
    for word in words:
        if word in registers64:
            if word == op:
                opBit = 'q'
            op = op.replace(word, 'q' + hex(registers64.index(word) * 8)[2:])
        elif word in registers32:
            if word == op:
                opBit = 'd'
            op = op.replace(word, 'd' + hex(registers32.index(word) * 8)[2:])
        elif word in registers16:
            if word == op:
                opBit = 'w'
            op = op.replace(word, 'w' + hex(registers16.index(word) * 8)[2:])
        elif word in registersLow8:
            if word == op:
                opBit = 'b'
            op = op.replace(word, 'b' + hex(registersLow8.index(word) * 8)[2:])

    op = re.sub(r'^(\[.+\])', lambda match: 'l' + match.group(1).replace('[', '').replace(']', ''), op)
    op = re.sub(r'(.+(\[.+\]))', lambda match: 'p' + match.group(2).replace('[', '').replace(']', ''), op)
    return opBit, op.replace(' ', '')

def FormatAsm(asm):
    for instruction in asm.split('\n'):
        info = instruction.split('_')
        if len(info) != 3:
            continue

        address = info[0].replace('0x', '')
        mnemonic = str(mnemonics.index(info[1]))

        # 确保有两个操作数
        ops = info[2].replace('0x', 'i').split(', ')
        if len(ops) == 1:
            ops += ['']

        opBit1, op1 = FormatOp(ops[0])
        opBit2, op2 = FormatOp(ops[1])
        # 无法直接确定 op1 位数的情况: 立即数
        if opBit1 == '?':
            opBit1 = 'q'
        # 无法直接确定 op2 位数的情况: lea、立即数、不存在
        if opBit2 == '?':
            opBit2 = opBit1

        print(address + '\\0""' + mnemonic + '\\0""' + opBit1 + '\\0""' + op1 + '\\0""' + opBit2 + '\\0""' + op2 + '\\0""', end='')

    mnemonicMapping = '!\n\nPVOID mnemonicMapping[] = { '
    for mnemonic in mnemonics:
        mnemonicMapping += mnemonic[0].upper() + mnemonic[1:] + ', '
    print(mnemonicMapping[:-2] + ' };\n')