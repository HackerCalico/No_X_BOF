from Disassembly import ParseShellCode
from GenerateSelfAsm import FormatAsm

if __name__ == '__main__':
    while True:
        choice = input('1.反汇编\n2.生成自定义汇编指令\n选择: ')

        if choice == '1':
            with open('ShellCode.txt', 'r') as file:
                shellcode = file.read().replace(' ', '').replace('\n', '')
            asm = ParseShellCode(shellcode)
            with open('asm.txt', 'w') as file:
                shellcode = file.write(asm)
            print('汇编指令生成完毕(未考虑负数，格式应为0x-1): asm.txt\n')

        elif choice == '2':
            with open('asm.txt', 'r') as file:
                asm = file.read()
            FormatAsm(asm)