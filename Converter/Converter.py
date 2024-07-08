from Imul import ProcessImul
from GenerateSelfAsm import FormatAsm
from Disassembly import ParseShellCode

if __name__ == '__main__':
    while True:
        choice = input('1.反汇编\n2.Imul转换\n3.生成自定义汇编指令\n选择: ')

        if choice == '1':
            with open('ShellCode.txt', 'r') as file:
                shellcode = file.read().replace(' ', '').replace('\n', '')
            asm = ParseShellCode(shellcode)
            with open('asm.txt', 'w') as file:
                file.write(asm)
            print('汇编指令生成完毕: asm.txt\n')

        elif choice == '2':
            with open('asm.txt', 'r') as file:
                asm = file.read()
            asm = ProcessImul(asm)
            with open('asm.txt', 'w') as file:
                file.write(asm)
            print('已将 asm.txt 中的 imul a, a, b 全部转换为 imul a, b\n')

        elif choice == '3':
            with open('asm.txt', 'r') as file:
                asm = file.read()
            FormatAsm(asm)