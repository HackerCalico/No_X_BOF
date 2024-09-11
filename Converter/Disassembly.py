import re
import struct
from capstone import Cs, CS_ARCH_X86, CS_MODE_64

from GeneratePayload import mnemonics

class IMAGE_FILE_HEADER:
    def __init__(self, data):
        self.Machine, \
        self.NumberOfSections, \
        self.TimeDateStamp, \
        self.PointerToSymbolTable, \
        self.NumberOfSymbols, \
        self.SizeOfOptionalHeader, \
        self.Characteristics \
            = struct.unpack('<HHIIIHH', data)

class IMAGE_SECTION_HEADER:
    def __init__(self, data):
        self.Name, \
        self.VirtualSize, \
        self.VirtualAddress, \
        self.SizeOfRawData, \
        self.PointerToRawData, \
        self.PointerToRelocations, \
        self.PointerToLinenumbers, \
        self.NumberOfRelocations, \
        self.NumberOfLinenumbers, \
        self.Characteristics \
            = struct.unpack('<8sIIIIIIHHI', data)

class IMAGE_RELOCATION:
    def __init__(self, data):
        self.VirtualAddress, \
        self.SymbolTableIndex, \
        self.Type, \
            = struct.unpack('<IIH', data)

class IMAGE_SYMBOL:
    def __init__(self, data):
        self.N, \
        self.Value, \
        self.SectionNumber, \
        self.Type, \
        self.StorageClass, \
        self.NumberOfAuxSymbols \
            = struct.unpack('<8sIHHBB', data)

def ParseCoff(coff):
    fileHeader = IMAGE_FILE_HEADER(coff[:struct.calcsize('<HHIIIHH')])

    for i in range(fileHeader.NumberOfSections):
        offset = struct.calcsize('<HHIIIHH') + i * struct.calcsize('<8sIIIIIIHHI')
        sectionHeader = IMAGE_SECTION_HEADER(coff[offset:offset+struct.calcsize('<8sIIIIIIHHI')])
        if sectionHeader.Name == b'.text\x00\x00\x00':
            textRelocNum = sectionHeader.NumberOfRelocations
            textRelocOffset = sectionHeader.PointerToRelocations
            text = coff[sectionHeader.PointerToRawData:sectionHeader.PointerToRawData+sectionHeader.SizeOfRawData]
        elif sectionHeader.Name == b'.rdata\x00\x00':
            rdata = coff[sectionHeader.PointerToRawData:sectionHeader.PointerToRawData+sectionHeader.SizeOfRawData]

    textRelocNameList = []
    stringTableOffset = fileHeader.PointerToSymbolTable + fileHeader.NumberOfSymbols * struct.calcsize('<8sIHHBB')
    for i in range(textRelocNum):
        offset = textRelocOffset + i * struct.calcsize('<IIH')
        reloc = IMAGE_RELOCATION(coff[offset:offset+struct.calcsize('<IIH')])
        offset = fileHeader.PointerToSymbolTable + reloc.SymbolTableIndex * struct.calcsize('<8sIHHBB')
        symbol = IMAGE_SYMBOL(coff[offset:offset+struct.calcsize('<8sIHHBB')])
        if symbol.N[:4] == b'\x00\x00\x00\x00':
            nameoffset = stringTableOffset + struct.unpack('<I', symbol.N[4:])[0]
            name = coff[nameoffset:].split(b'\0')[0]
            if b'$' in name:
                textRelocNameList += [name[6:].decode()]
            else:
                raise Exception('不支持对 .text 的 ' + name.decode() + ' 重定位')
        elif symbol.N == b'.rdata\x00\x00':
            textRelocNameList += ['.rdata']
        else:
            raise Exception('不支持对 .text 的 ' + symbol.N.split(b'\0')[0].decode() + ' 重定位')

    bofFuncOffsetDict = dict()
    for i in range(fileHeader.NumberOfSymbols):
        offset = fileHeader.PointerToSymbolTable + i * struct.calcsize('<8sIHHBB')
        symbol = IMAGE_SYMBOL(coff[offset:offset+struct.calcsize('<8sIHHBB')])
        if symbol.N[:4] == b'\x00\x00\x00\x00':
            nameoffset = stringTableOffset + struct.unpack('<I', symbol.N[4:])[0]
            name = coff[nameoffset:].split(b'\0')[0]
            if b'$$' in name:
                bofFuncOffsetDict[name.decode()] = symbol.Value
        elif b'$$' in symbol.N:
            bofFuncOffsetDict[symbol.N.split(b'\0')[0].decode()] = symbol.Value
    if len(bofFuncOffsetDict) == 0:
        raise Exception('未找到可调用 BOF 函数, 函数名需包含 $$')

    return text, rdata, textRelocNameList, bofFuncOffsetDict

def Disassembly(coff):
    text, rdata, textRelocNameList, bofFuncOffsetDict = ParseCoff(coff)

    cs = Cs(CS_ARCH_X86, CS_MODE_64)
    instructions = cs.disasm(text, 0)

    asm = ''
    useMnemonics = set()
    for instruction in instructions:
        mnemonic = instruction.mnemonic.replace('rep stosb', 'repStosb').replace('rep movsb', 'repMovsb')
        useMnemonics.add(mnemonic)
        ops = re.sub(r'(-?\b[0-9a-fA-F]+\b)', r'0x\1', instruction.op_str) # 补全立即数 0x 前缀
        asm += hex(instruction.address) + '_' + mnemonic + '_' + ops + '\n'

    print('BOF 使用的汇编指令: ' + str(useMnemonics))

    notExistMnemonics = []
    for useMnemonic in useMnemonics:
        if useMnemonic not in mnemonics:
            notExistMnemonics += [useMnemonic]
    if len(notExistMnemonics) > 0:
        raise Exception('未实现的自定义汇编指令: ' + str(notExistMnemonics) + '\n' + asm)

    return asm, rdata, textRelocNameList, bofFuncOffsetDict