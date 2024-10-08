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

def GetHash(bytes):
    hash = 0
    for byte in bytes:
        hash += byte
        hash = (hash << 8) - hash
    hash &= 0xffffffff # 保留 32 位
    if hash & 0x80000000: # 负数
        hash = -((~hash + 1) & 0xffffffff)
    return hash

def ParseBOF(coff):
    fileHeader = IMAGE_FILE_HEADER(coff[:struct.calcsize('<HHIIIHH')])

    text = rdata = textRelocNum = textRelocOffset = None
    for i in range(fileHeader.NumberOfSections):
        offset = struct.calcsize('<HHIIIHH') + i * struct.calcsize('<8sIIIIIIHHI')
        sectionHeader = IMAGE_SECTION_HEADER(coff[offset:offset+struct.calcsize('<8sIIIIIIHHI')])
        if sectionHeader.Name == b'.text\x00\x00\x00':
            textRelocNum = sectionHeader.NumberOfRelocations
            textRelocOffset = sectionHeader.PointerToRelocations
            text = coff[sectionHeader.PointerToRawData:sectionHeader.PointerToRawData+sectionHeader.SizeOfRawData]
        elif sectionHeader.Name == b'.rdata\x00\x00':
            rdata = coff[sectionHeader.PointerToRawData:sectionHeader.PointerToRawData+sectionHeader.SizeOfRawData]
    if text == None:
        raise Exception('未找到 .text')
    if rdata == None:
        raise Exception('未找到 .rdata')

    textRelocNames = b''
    stringTableOffset = fileHeader.PointerToSymbolTable + fileHeader.NumberOfSymbols * struct.calcsize('<8sIHHBB')
    for i in range(textRelocNum):
        offset = textRelocOffset + i * struct.calcsize('<IIH')
        reloc = IMAGE_RELOCATION(coff[offset:offset+struct.calcsize('<IIH')])
        offset = fileHeader.PointerToSymbolTable + reloc.SymbolTableIndex * struct.calcsize('<8sIHHBB')
        symbol = IMAGE_SYMBOL(coff[offset:offset+struct.calcsize('<8sIHHBB')])
        if symbol.N[:4] == b'\x00\x00\x00\x00':
            nameoffset = stringTableOffset + struct.unpack('<I', symbol.N[4:])[0]
            name = coff[nameoffset:].split(b'\0')[0]
            if b'$$' in name:
                print('\033[91m' + f'[!] 请手动对 Disassembly\\asm.txt 中调用 {name.decode()} 的 call [地址] 进行重定位' + '\033[0m')
            elif b'$' in name:
                textRelocNames += name[6:] + b'\x00'
            else:
                raise Exception('不支持对 .text 的 ' + name.decode() + ' 重定位')
        elif symbol.N == b'.rdata\x00\x00':
            textRelocNames += b'.rdata\x00'
        else:
            raise Exception('不支持对 .text 的 ' + symbol.N.split(b'\0')[0].decode() + ' 重定位')

    bofFuncOffsetMap = b''
    for i in range(fileHeader.NumberOfSymbols):
        offset = fileHeader.PointerToSymbolTable + i * struct.calcsize('<8sIHHBB')
        symbol = IMAGE_SYMBOL(coff[offset:offset+struct.calcsize('<8sIHHBB')])
        if symbol.N[:4] == b'\x00\x00\x00\x00':
            nameoffset = stringTableOffset + struct.unpack('<I', symbol.N[4:])[0]
            name = coff[nameoffset:].split(b'\0')[0]
            if b'$$' in name:
                print(name, 'Hash:', GetHash(name))
                bofFuncOffsetMap += struct.pack('<i', GetHash(name)) + struct.pack('<H', symbol.Value)
        elif b'$$' in symbol.N:
            print(symbol.N.split(b'\0')[0], 'Hash:', GetHash(symbol.N.split(b'\0')[0]))
            bofFuncOffsetMap += struct.pack('<i', GetHash(symbol.N.split(b'\0')[0])) + struct.pack('<H', symbol.Value)
    if len(bofFuncOffsetMap) == 0:
        raise Exception('未找到 BOF 函数, 函数名需包含 $$')

    return text, rdata, textRelocNames, bofFuncOffsetMap

def Disassembly(bof):
    text, rdata, textRelocNames, bofFuncOffsetMap = ParseBOF(bof)

    cs = Cs(CS_ARCH_X86, CS_MODE_64)
    instructions = cs.disasm(text, 0)

    asm = ''
    useMnemonics = set()
    for instruction in instructions:
        useMnemonics.add(instruction.mnemonic)
        ops = re.sub(r'(-?\b[0-9a-fA-F]+\b)', r'0x\1', instruction.op_str) # 补全立即数 0x 前缀
        asm += hex(instruction.address) + '_' + instruction.mnemonic + '_' + ops + '\n'
    print('BOF 使用的汇编指令: ' + str(useMnemonics))

    notExistMnemonics = []
    for useMnemonic in useMnemonics:
        if useMnemonic not in mnemonics:
            notExistMnemonics += [useMnemonic]
    if len(notExistMnemonics) > 0:
        raise Exception('未实现的自定义指令: ' + str(notExistMnemonics) + '\n' + asm)
    return asm, rdata, textRelocNames, bofFuncOffsetMap