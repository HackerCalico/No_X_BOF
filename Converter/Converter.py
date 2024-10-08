from Disassembly import Disassembly
from GeneratePayload import GeneratePayload

if __name__ == '__main__':
    print('''███╗   ██╗ ██████╗    ██╗  ██╗   ██████╗  ██████╗ ███████╗
████╗  ██║██╔═══██╗   ╚██╗██╔╝   ██╔══██╗██╔═══██╗██╔════╝
██╔██╗ ██║██║   ██║    ╚███╔╝    ██████╔╝██║   ██║███████╗  
██║╚██╗██║██║   ██║    ██╔██╗    ██╔══██╗██║   ██║██╔════╝
██║ ╚████║╚██████╔╝   ██╔╝ ██╗   ██████╔╝╚██████╔╝██║     
╚═╝  ╚═══╝ ╚═════╝    ╚═╝  ╚═╝   ╚═════╝  ╚═════╝ ╚═╝     
https://github.com/HackerCalico/No_X_BOF''')
    print('1.反汇编\n2.生成 Payload')
    choice = input('\033[94m' + 'Choice: ' + '\033[0m')

    if choice == '1':
        with open('..\\BOF\\bof.o', 'rb') as file:
            bof = file.read()

        asm, rdata, textRelocNames, bofFuncOffsetMap = Disassembly(bof)

        with open('Disassembly\\asm.txt', 'w', encoding='UTF-8') as file:
            file.write(asm)
        with open('Disassembly\\rdata.bin', 'wb') as file:
            file.write(rdata)
        with open('Disassembly\\textRelocNames.bin', 'wb') as file:
            file.write(textRelocNames)
        with open('Disassembly\\bofFuncOffsetMap.bin', 'wb') as file:
            file.write(bofFuncOffsetMap)
        print('\033[92m' + '[+] 反汇编结果生成至文件夹 Disassembly\n' + '\033[0m')

    elif choice == '2':
        with open('Disassembly\\asm.txt', 'r', encoding='UTF-8') as file:
            asm = file.read()
        with open('Disassembly\\rdata.bin', 'rb') as file:
            rdata = file.read()
        with open('Disassembly\\textRelocNames.bin', 'rb') as file:
            textRelocNames = file.read()
        with open('Disassembly\\bofFuncOffsetMap.bin', 'rb') as file:
            bofFuncOffsetMap = file.read()

        payload = GeneratePayload(asm, rdata, textRelocNames, bofFuncOffsetMap)

        with open('Payload.bin', 'wb') as file:
            file.write(payload)
        print('\033[92m' + '[+] Payload 生成至 Payload.bin\n' + '\033[0m')