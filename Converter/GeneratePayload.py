import re
import random
import string

registers64 = ['rax', 'rbx', 'rcx', 'rdx', 'rsi', 'rdi', 'r8', 'r9', 'r10', 'r11', 'r12', 'r13', 'r14', 'r15', 'rsp', 'rbp']
registers32 = ['eax', 'ebx', 'ecx', 'edx', 'esi', 'edi', 'r8d', 'r9d', 'r10d', 'r11d', 'r12d', 'r13d', 'r14d', 'r15d', 'esp', 'ebp']
registers16 = ['ax', 'bx', 'cx', 'dx', 'si', 'di', 'r8w', 'r9w', 'r10w', 'r11w', 'r12w', 'r13w', 'r14w', 'r15w', 'sp', 'bp']
registersLow8 = ['al', 'bl', 'cl', 'dl', 'sil', 'dil', 'r8b', 'r9b', 'r10b', 'r11b', 'r12b', 'r13b', 'r14b', 'r15b', 'spl', 'bpl']

mnemonics = ['push', 'pop', 'call', 'ret', 'mov', 'movzx', 'movabs', 'movsxd', 'lea', 'nop', 'add', 'inc', 'sub', 'cmp', 'imul', 'and', 'or', 'xor', 'test', 'shl', 'shr', 'cdqe', 'repStosb', 'repMovsb', 'jmp', 'je', 'jne', 'jbe', 'jl', 'jge', 'jle']

obfDict = {
    'q' : 'q',
    'd' : 'd',
    'w' : 'w',
    'b' : 'b',
    'i' : 'i',
    'r' : 'r', # RIP
    'p' : 'p', # ptr
    'l' : 'l', # lea
    '+' : '+',
    '-' : '~', # 避免与负数冲突
    '_' : '_'
}

def FormatOp(op):
    # 不存在 / 立即数
    if op == '' or op[0] == obfDict['i']:
        return '?', op

    # 寄存器 -> 相对偏移
    if '[' not in op:
        if op == 'rip':
            return obfDict['q'], obfDict['r'] + '0000000000000000'
        if op in registers64:
            return obfDict['q'], obfDict['q'] + hex(registers64.index(op) * 8)[2:]
        if op in registers32:
            return obfDict['d'], obfDict['d'] + hex(registers32.index(op) * 8)[2:]
        if op in registers16:
            return obfDict['w'], obfDict['w'] + hex(registers16.index(op) * 8)[2:]
        if op in registersLow8:
            return obfDict['b'], obfDict['b'] + hex(registersLow8.index(op) * 8)[2:]

    opBit = '?'
    if 'ptr' in op:
        if 'qword' in op:
            opBit = obfDict['q']
        elif 'dword' in op:
            opBit = obfDict['d']
        elif 'word' in op:
            opBit = obfDict['w']
        elif 'byte' in op:
            opBit = obfDict['b']

    # [寄存器] -> [相对偏移]
    words = op.replace('[', '').replace(']', '').split(' ')
    for word in words:
        if word == 'rip':
            op = op.replace(word, obfDict['r'] + '0000000000000000')
        elif word in registers64:
            op = op.replace(word, obfDict['q'] + hex(registers64.index(word) * 8)[2:])
        elif word in registers32:
            op = op.replace(word, obfDict['d'] + hex(registers32.index(word) * 8)[2:])
        elif word in registers16:
            op = op.replace(word, obfDict['w'] + hex(registers16.index(word) * 8)[2:])
        elif word in registersLow8:
            op = op.replace(word, obfDict['b'] + hex(registersLow8.index(word) * 8)[2:])

    # [相对偏移] -> l相对偏移
    op = re.sub(r'^(\[.+\])', lambda match: obfDict['l'] + match.group(1).replace('[', '').replace(']', ''), op)
    # xxx ptr [相对偏移] -> p相对偏移
    op = re.sub(r'.+(\[.+\])', lambda match: obfDict['p'] + match.group(1).replace('[', '').replace(']', ''), op)
    return opBit, op.replace(' ', '')

def FormatAsm(asm):
    if '*' in asm:
        raise Exception('无法处理 [] 中的 *')
    asm = asm.replace('+', obfDict['+']).replace('- ', obfDict['-'] + ' ')

    selfAsm = ''
    for instruction in asm.split('\n'):
        asmInfo = instruction.split('_')
        if len(asmInfo) != 3:
            if len(instruction) != 0:
                raise Exception('无法处理汇编指令: ' + instruction)
            # 末尾的空行
            else:
                continue

        address = asmInfo[0].replace('0x', '')
        mnemonic = str(mnemonics.index(asmInfo[1]))

        # 确保有两个操作数, 0x立即数 -> i立即数
        ops = asmInfo[2].replace('0x', obfDict['i']).split(', ')
        if len(ops) == 1:
            ops += ['']
        if len(ops) != 2:
            raise Exception('只能处理 1 ~ 2 个操作数的汇编指令: ' + instruction)

        opBit1, op1 = FormatOp(ops[0])
        opBit2, op2 = FormatOp(ops[1])
        # 无法直接确定 op1 位数的情况: 立即数、不存在
        if opBit1 == '?':
            opBit1 = obfDict['q']
        # 无法直接确定 op2 位数的情况: lea、立即数、不存在
        if opBit2 == '?':
            opBit2 = opBit1

        selfAsm += address + obfDict['_'] + mnemonic + obfDict['_'] + opBit1 + obfDict['_'] + op1 + obfDict['_'] + opBit2 + obfDict['_'] + op2 + obfDict['_']
    return selfAsm

def GeneratePayload(asm, rdata, textRelocNameList, bofFuncOffsetDict, isObf):
    # 随机生成自定义汇编关键字
    global obfDict
    if isObf:
        differChars = random.sample(string.ascii_uppercase + string.digits, k=11)
        i = 0
        for key, value in obfDict.items():
            obfDict[key] = differChars[i]
            i += 1
        differChars = random.sample('!#$%&./:;<=>?@^`|~', k=3)
        obfDict['+'] = differChars[0]
        obfDict['-'] = differChars[1]
        obfDict['_'] = differChars[2]

    selfAsm = FormatAsm(asm)

    obfDict2 = dict()
    for key, value in obfDict.items():
        obfDict2[str(hex(ord(key))[2:])] = value

    mnemonicMap = '请手动在 Instruction.cpp 的 InvokeInstruction 函数开头定义:\n' \
                  'int jmpIndex = ' + str(mnemonics.index('jmp')) + ';\n'\
                  'PVOID mnemonicMap[] = { '
    for mnemonic in mnemonics:
        mnemonicMap += mnemonic[0].upper() + mnemonic[1:] + ', '
    print(mnemonicMap[:-2] + ' };')

    return bofFuncOffsetDict.encode() + b'\x00' + textRelocNameList.encode() + b'\x00' + str(obfDict2).replace('\'', '"').encode() + b'\x00' + selfAsm.encode() + b'\x00' + rdata