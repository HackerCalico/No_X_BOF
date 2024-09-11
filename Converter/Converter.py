from Simplify import SimplifyImul
from Disassembly import Disassembly
from GeneratePayload import GeneratePayload

if __name__ == '__main__':
    try:
        choice = input('1.反汇编\n2.生成 Payload\n3.生成未混淆 Payload (实验)\n选择: ')

        if choice == '1':
            with open('..\\BOF\\bof.o', 'rb') as file:
                coff = file.read()

            asm, rdata, textRelocNameList, bofFuncOffsetDict = Disassembly(coff)
            asm = SimplifyImul(asm)

            with open('Disassembly\\asm.txt', 'w', encoding='UTF-8') as file:
                file.write(asm)
            with open('Disassembly\\rdata.bin', 'wb') as file:
                file.write(rdata)
            with open('Disassembly\\textRelocNameList.txt', 'w', encoding='UTF-8') as file:
                file.write(str(textRelocNameList).replace('\'', '"'))
            with open('Disassembly\\bofFuncOffsetDict.txt', 'w', encoding='UTF-8') as file:
                file.write(str(bofFuncOffsetDict).replace('\'', '"'))
            print('反汇编结果生成至文件夹: Disassembly\n')

        elif choice == '2' or choice == '3':
            with open('Disassembly\\asm.txt', 'r', encoding='UTF-8') as file:
                asm = file.read()
            with open('Disassembly\\rdata.bin', 'rb') as file:
                rdata = file.read()
            with open('Disassembly\\textRelocNameList.txt', 'r', encoding='UTF-8') as file:
                textRelocNameList = file.read()
            with open('Disassembly\\bofFuncOffsetDict.txt', 'r', encoding='UTF-8') as file:
                bofFuncOffsetDict = file.read()

            if choice == '2':
                payload = GeneratePayload(asm, rdata, textRelocNameList, bofFuncOffsetDict, True)
            else:
                payload = GeneratePayload(asm, rdata, textRelocNameList, bofFuncOffsetDict, False)

            with open('Payload.bin', 'wb') as file:
                file.write(payload)
            print('Payload (BOF 函数偏移 + .text 重定位名称 + 混淆映射 + 自定义汇编 + .rdata) 生成至: Payload.bin\n')
    except Exception as e:
        print('\033[91m' + str(e) + '\033[0m')